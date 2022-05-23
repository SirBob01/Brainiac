#include "board.h"

namespace brainiac {
    Board::Board(std::string fen_string) {
        std::vector<std::string> fields =
            brainiac::util::tokenize(fen_string, ' ');
        _states.reserve(64);
        _states.emplace_back();
        _current_state = 0;
        BoardState &state = _states[_current_state];

        int row = 7;
        int col = 0;
        for (auto &c : fields[0]) {
            if (c == '/') {
                row--;
                col = 0;
            } else if (c <= '9' && c >= '0') {
                col += c - '0';
            } else {
                int char_idx = 0;
                while (piece_chars[char_idx] != c) {
                    char_idx++;
                }
                Square sq = row * 8 + col;
                PieceType type =
                    static_cast<PieceType>(char_idx % PieceType::NPieces);
                Color color = static_cast<Color>(char_idx / PieceType::NPieces);
                Piece piece = {type, color};
                set_at(sq, piece);
                state._material += piece.get_weight();
                col++;
            }
        }

        _turn = (fields[1][0] == 'w') ? Color::White : Color::Black;
        state._castling_rights = 0;
        for (auto &c : fields[2]) {
            if (c == 'K') state._castling_rights |= Castle::WK;
            else if (c == 'Q') state._castling_rights |= Castle::WQ;
            else if (c == 'k') state._castling_rights |= Castle::BK;
            else if (c == 'q') state._castling_rights |= Castle::BQ;
        }

        if (fields[3].length() == 2) {
            state._en_passant_target = string_to_square(fields[3]);
        }
        state._halfmoves = stoi(fields[4]);
        _fullmoves = stoi(fields[5]);

        state._attackers = get_attackers();
        generate_moves();

        // Calculate the Zobrist hash
        state._hash = zobrist_hash(_turn,
                                   state._bitboards,
                                   state._castling_rights,
                                   state._en_passant_target);
    }

    bool Board::is_legal(Move &move) {
        Piece king = {PieceType::King, _turn};
        BoardState &state = _states[_current_state];
        uint64_t kingbit = state._bitboards[king.get_index()];

        Square from = move.get_from();
        Square to = move.get_to();
        MoveFlagSet flags = move.get_flags();

        uint64_t from_mask = get_square_mask(from);
        uint64_t to_mask = get_square_mask(to);

        // Handle en passant
        if (flags & MoveFlag::EnPassant) {
            int rankd = to - from;
            int dir = (rankd > 0) - (rankd < 0);
            Square target_pawn(state._en_passant_target - (dir * 8));

            return (get_attackers(to_mask,
                                  from_mask,
                                  get_square_mask(target_pawn)) &
                    kingbit) == 0;
        }

        // Handle castling
        if (flags & MoveFlag::Castling) {
            int rankd = to - from;
            int dir = (rankd > 0) - (rankd < 0);
            Square pass_through(to - dir);
            if ((from_mask & state._attackers) ||
                (state._attackers & get_square_mask(pass_through)) ||
                (to_mask & state._attackers)) {
                return false;
            }
            return true;
        }

        // Attacked squares after move has been made
        uint64_t new_attackers = get_attackers(to_mask, from_mask);
        if (from_mask & kingbit) {
            return (new_attackers & to_mask) == 0;
        }
        return (new_attackers & kingbit) == 0;
    }

    void Board::register_move(Move &move) {
        BoardState &state = _states[_current_state];
        if (is_legal(move)) {
            state._legal_moves.push_back(move);
        }
    }

    void Board::generate_pawn_moves(uint64_t bitboard) {
        BoardState &state = _states[_current_state];
        uint64_t allies = state._bitboards[PieceType::NPieces * 2 + _turn];
        uint64_t enemies = state._bitboards[PieceType::NPieces * 2 + !_turn];
        uint64_t all_pieces = allies | enemies;
        uint32_t piece_shift = find_lsb(bitboard);

        // Pre-compute advance, capture, and en-passant masks
        uint64_t advance_board =
            get_pawn_advance_mask(bitboard, all_pieces, _turn);
        uint64_t double_board =
            get_pawn_double_mask(bitboard, all_pieces, _turn);
        uint64_t capture_mask = get_pawn_capture_mask(bitboard, _turn);
        uint64_t en_passant_mask = 0;
        if (!is_square_invalid(state._en_passant_target)) {
            en_passant_mask = get_square_mask(state._en_passant_target);
        }

        // Single pawn advance
        while (advance_board) {
            uint64_t move = advance_board & (-advance_board);
            if (move & end_ranks) {
                for (int i = 0; i < 4; i++) {
                    Move moveobj = {piece_shift,
                                    find_lsb(move),
                                    pawn_single_flags | promotions[i]};
                    register_move(moveobj);
                }
            } else {
                Move moveobj = {piece_shift, find_lsb(move), pawn_single_flags};
                register_move(moveobj);
            }
            advance_board &= (advance_board - 1);
        }

        // Double pawn advance
        while (double_board) {
            uint64_t move = double_board & (-double_board);
            Move moveobj = {piece_shift, find_lsb(move), pawn_double_flags};
            register_move(moveobj);
            double_board &= (double_board - 1);
        }

        // Regular captures
        uint64_t capture_board = capture_mask & enemies;
        while (capture_board) {
            uint64_t move = capture_board & (-capture_board);
            if (move & end_ranks) {
                for (int i = 0; i < 4; i++) {
                    Move moveobj = {piece_shift,
                                    find_lsb(move),
                                    pawn_capture_flags | promotions[i]};
                    register_move(moveobj);
                }
            } else {
                Move moveobj = {piece_shift,
                                find_lsb(move),
                                pawn_capture_flags};
                register_move(moveobj);
            }
            capture_board &= (capture_board - 1);
        }

        // En-passant captures
        uint64_t ep_board = capture_mask & en_passant_mask;
        while (ep_board) {
            uint64_t move = ep_board & (-ep_board);
            Move moveobj = {piece_shift, find_lsb(move), en_passant_flags};
            register_move(moveobj);
            ep_board &= (ep_board - 1);
        }
    }

    void Board::generate_step_moves(uint64_t bitboard,
                                    bool is_king,
                                    uint64_t (*mask_func)(uint64_t)) {
        BoardState &state = _states[_current_state];
        uint64_t allies = state._bitboards[PieceType::NPieces * 2 + _turn];
        uint64_t enemies = state._bitboards[PieceType::NPieces * 2 + !_turn];
        uint32_t piece_shift = find_lsb(bitboard);

        // King cannot move in range of his attackers
        uint64_t moves = mask_func(bitboard) & ~allies;
        if (is_king) {
            moves &= ~state._attackers;
        }

        uint64_t capture_board = moves & enemies;
        uint64_t quiet_board = moves & ~enemies;
        while (quiet_board) {
            uint64_t move = quiet_board & (-quiet_board);
            Move moveobj = {piece_shift, find_lsb(move), 0};
            register_move(moveobj);
            quiet_board &= (quiet_board - 1);
        }
        while (capture_board) {
            uint64_t move = capture_board & (-capture_board);
            Move moveobj = {piece_shift, find_lsb(move), MoveFlag::Capture};
            register_move(moveobj);
            capture_board &= (capture_board - 1);
        }
    }

    void Board::generate_slider_moves(uint64_t bitboard,
                                      uint64_t (*mask_func)(uint64_t,
                                                            uint64_t,
                                                            uint64_t)) {
        BoardState &state = _states[_current_state];
        uint64_t allies = state._bitboards[PieceType::NPieces * 2 + _turn];
        uint64_t enemies = state._bitboards[PieceType::NPieces * 2 + !_turn];
        uint32_t piece_shift = find_lsb(bitboard);

        uint64_t moves = mask_func(bitboard, allies, enemies);

        uint64_t capture_board = moves & enemies;
        uint64_t quiet_board = moves & ~enemies;
        while (quiet_board) {
            uint64_t move = quiet_board & (-quiet_board);
            Move moveobj = {piece_shift, find_lsb(move), 0};
            register_move(moveobj);
            quiet_board &= (quiet_board - 1);
        }
        while (capture_board) {
            uint64_t move = capture_board & (-capture_board);
            Move moveobj = {piece_shift, find_lsb(move), MoveFlag::Capture};
            register_move(moveobj);
            capture_board &= (capture_board - 1);
        }
    }

    void Board::generate_castling_moves(uint64_t bitboard) {
        BoardState &state = _states[_current_state];
        uint64_t allies = state._bitboards[PieceType::NPieces * 2 + _turn];
        uint64_t enemies = state._bitboards[PieceType::NPieces * 2 + !_turn];
        uint64_t all_pieces = allies | enemies;

        uint8_t rights = state._castling_rights & color_castling_rights[_turn];
        Square from(find_lsb(bitboard));
        while (rights) {
            Castle side = static_cast<Castle>(rights & (-rights));

            // King cannot move in range of his attackers
            uint64_t mask =
                get_castling_mask(all_pieces, side) & ~state._attackers;
            if (mask) {
                Square to(find_lsb(mask));
                Move move = {from, to, MoveFlag::Castling};
                register_move(move);
            }
            rights &= (rights - 1);
        }
    }

    uint64_t Board::get_attackers(uint64_t allies_include,
                                  uint64_t allies_exclude,
                                  uint64_t enemies_exclude) {
        // Generate attack vectors for sliding pieces to test if king is in
        // check
        BoardState &state = _states[_current_state];
        Color opponent = static_cast<Color>(!_turn);

        Piece king = {PieceType::King, _turn};
        uint64_t source_mask = ~enemies_exclude & ~allies_include;
        uint64_t source_squares =
            state._bitboards[PieceType::NPieces * 2 + opponent] & source_mask;
        uint64_t target_squares =
            (state._bitboards[PieceType::NPieces * 2 + _turn] |
             allies_include) &
            ~state._bitboards[king.get_index()] & ~allies_exclude;

        uint64_t attacked = 0;
        for (int type = 0; type < PieceType::NPieces; type++) {
            Piece piece = {static_cast<PieceType>(type), opponent};
            uint64_t bitboard =
                state._bitboards[piece.get_index()] & source_mask;

            switch (piece.type) {
            case PieceType::Pawn:
                attacked |= get_pawn_capture_mask(bitboard, opponent);
                break;
            case PieceType::Knight:
                attacked |= get_knight_mask(bitboard) & ~source_squares;
                break;
            case PieceType::King:
                attacked |= get_king_mask(bitboard) & ~source_squares;
                break;
            default:
                switch (piece.type) {
                case PieceType::Bishop:
                    while (bitboard) {
                        uint64_t unit = bitboard & (-bitboard);
                        attacked |= get_bishop_mask(unit,
                                                    source_squares,
                                                    target_squares);
                        bitboard &= (bitboard - 1);
                    }
                    break;
                case PieceType::Rook:
                    while (bitboard) {
                        uint64_t unit = bitboard & (-bitboard);
                        attacked |=
                            get_rook_mask(unit, source_squares, target_squares);
                        bitboard &= (bitboard - 1);
                    }
                    break;
                case PieceType::Queen:
                    while (bitboard) {
                        uint64_t unit = bitboard & (-bitboard);
                        attacked |= get_queen_mask(unit,
                                                   source_squares,
                                                   target_squares);
                        bitboard &= (bitboard - 1);
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        }
        return attacked;
    }

    void Board::generate_moves() {
        BoardState &state = _states[_current_state];
        state._legal_moves.clear();
        for (int type = 0; type < PieceType::NPieces; type++) {
            Piece piece = {static_cast<PieceType>(type), _turn};
            uint64_t bitboard = state._bitboards[piece.get_index()];
            switch (type) {
            case PieceType::Pawn:
                while (bitboard) {
                    generate_pawn_moves(bitboard & (-bitboard));
                    bitboard &= (bitboard - 1);
                }
                break;
            case PieceType::King:
                while (bitboard) {
                    uint64_t unit = bitboard & (-bitboard);
                    generate_step_moves(unit, true, get_king_mask);
                    generate_castling_moves(unit);
                    bitboard &= (bitboard - 1);
                }
                break;
            case PieceType::Knight:
                while (bitboard) {
                    generate_step_moves(bitboard & (-bitboard),
                                        false,
                                        get_knight_mask);
                    bitboard &= (bitboard - 1);
                }
                break;
            case PieceType::Bishop:
                while (bitboard) {
                    generate_slider_moves(bitboard & (-bitboard),
                                          get_bishop_mask);
                    bitboard &= (bitboard - 1);
                }
                break;
            case PieceType::Rook:
                while (bitboard) {
                    generate_slider_moves(bitboard & (-bitboard),
                                          get_rook_mask);
                    bitboard &= (bitboard - 1);
                }
                break;
            case PieceType::Queen:
                while (bitboard) {
                    generate_slider_moves(bitboard & (-bitboard),
                                          get_queen_mask);
                    bitboard &= (bitboard - 1);
                }
                break;
            default:
                break;
            }
        }
    }

    BoardState &Board::push_state() {
        // If executing a new move while in undo state, overwrite future history
        _states.resize(_current_state + 1);
        _states.emplace_back(_states[_current_state]);
        _current_state++;

        BoardState &state = _states[_current_state];
        state._halfmoves++;
        return state;
    }

    std::string Board::generate_fen() {
        BoardState &state = _states[_current_state];
        std::string fen = "";
        for (int row = 7; row >= 0; row--) {
            int counter = 0;
            for (int col = 0; col < 8; col++) {
                Piece piece = get_at_coords(row, col);
                if (!piece.is_empty()) {
                    if (counter) {
                        fen += counter + '0';
                        counter = 0;
                    }
                    fen += piece.get_char();
                } else {
                    counter += 1;
                }
            }
            if (counter) fen += counter + '0';
            if (row) fen += '/';
        }
        fen += " ";
        fen += _turn == Color::White ? "w" : "b";

        std::string castling_rights = "";

        if (state._castling_rights & Castle::WK) castling_rights += 'K';
        if (state._castling_rights & Castle::WQ) castling_rights += 'Q';
        if (state._castling_rights & Castle::BK) castling_rights += 'k';
        if (state._castling_rights & Castle::BQ) castling_rights += 'q';
        if (castling_rights.length() == 0) castling_rights = "-";
        fen += " " + castling_rights;

        if (is_square_invalid(state._en_passant_target)) {
            fen += " -";
        } else {
            fen += " ";
            fen += square_to_string(state._en_passant_target);
        }

        fen += " ";
        fen += std::to_string(state._halfmoves);
        fen += " ";
        fen += std::to_string(_fullmoves);
        return fen;
    }

    Piece Board::get_at(const Square sq) {
        BoardState &state = _states[_current_state];
        uint64_t mask = get_square_mask(sq);
        for (int idx = 0; idx < 12; idx++) {
            if (state._bitboards[idx] & mask) {
                PieceType type =
                    static_cast<PieceType>(idx % PieceType::NPieces);
                Color color = static_cast<Color>(idx / PieceType::NPieces);
                return {type, color};
            }
        }
        return {};
    }

    void Board::set_at(const Square sq, const Piece &piece) {
        clear_at(sq);
        uint64_t mask = get_square_mask(sq);

        BoardState &state = _states[_current_state];
        state._bitboards[2 * PieceType::NPieces + piece.color] |= mask;
        state._bitboards[piece.get_index()] |= mask;
    }

    Piece Board::get_at_coords(int row, int col) {
        return get_at(row * 8 + col);
    }

    void Board::set_at_coords(int row, int col, const Piece &piece) {
        set_at(row * 8 + col, piece);
    }

    void Board::clear_at(const Square sq) {
        // Clear all bitboards at this Square
        BoardState &state = _states[_current_state];
        uint64_t mask = ~(get_square_mask(sq));
        state._bitboards[12] &= mask;
        state._bitboards[13] &= mask;

        for (uint8_t piece = 0; piece < 14; piece++) {
            if (state._bitboards[piece] & get_square_mask(sq)) {
                state._bitboards[piece] &= mask;
                break;
            }
        }
    }

    void Board::skip_turn() {
        BoardState &state = push_state();

        // Null move does not do anything, current turn is skipped
        if (_turn == Color::Black) {
            _fullmoves++;
        }
        _turn = static_cast<Color>(!_turn);
        state._hash ^= turn_bitstring;

        state._attackers = get_attackers();
        generate_moves();
    }

    void Board::make_move(const Move &move) {
        BoardState &state = push_state();

        Square from = move.get_from();
        Square to = move.get_to();
        MoveFlagSet flags = move.get_flags();

        Piece piece = get_at(from);
        Piece target = get_at(to);

        // Update material if move was a capture
        if (!target.is_empty()) {
            state._material -= piece_weights[target.get_index()];
        }

        // Unset castling flags if relevant pieces were moved
        Castle queen_side =
            (_turn == Color::White) ? (Castle::WQ) : (Castle::BQ);
        Castle king_side =
            (_turn == Color::White) ? (Castle::WK) : (Castle::BK);

        Castle opp_queen_side =
            (_turn == Color::Black) ? (Castle::WQ) : (Castle::BQ);
        Castle opp_king_side =
            (_turn == Color::Black) ? (Castle::WK) : (Castle::BK);

        if (state._castling_rights & (king_side | queen_side)) {
            if (piece.type == PieceType::King) {
                if (state._castling_rights & king_side) {
                    state._hash ^= castling_bitstrings[find_lsb(king_side)];
                }
                if (state._castling_rights & queen_side) {
                    state._hash ^= castling_bitstrings[find_lsb(queen_side)];
                }
                state._castling_rights &= ~(king_side | queen_side);
            } else if (piece.type == PieceType::Rook) {
                uint64_t mask = get_square_mask(from);
                if (mask & fileA) {
                    if (state._castling_rights & queen_side) {
                        state._hash ^=
                            castling_bitstrings[find_lsb(queen_side)];
                    }
                    state._castling_rights &= ~queen_side;
                } else if (mask & fileH) {
                    if (state._castling_rights & king_side) {
                        state._hash ^= castling_bitstrings[find_lsb(king_side)];
                    }
                    state._castling_rights &= ~king_side;
                }
            }
        }

        // Unset castling opponent flags if pieces were captured
        if (target.type == PieceType::Rook) {
            uint64_t mask = get_square_mask(to);
            uint64_t rank = (_turn == Color::White) ? rank8 : rank1;
            if (mask & fileA & rank) {
                if (state._castling_rights & opp_queen_side) {
                    state._hash ^=
                        castling_bitstrings[find_lsb(opp_queen_side)];
                }
                state._castling_rights &= ~opp_queen_side;
            } else if (mask & fileH & rank) {
                if (state._castling_rights & opp_king_side) {
                    state._hash ^= castling_bitstrings[find_lsb(opp_king_side)];
                }
                state._castling_rights &= ~opp_king_side;
            }
        }

        // Move to target square and handle promotions
        clear_at(from);
        state._hash ^= zobrist_bitstring(piece, from);
        if (!target.is_empty()) {
            state._hash ^= zobrist_bitstring(target, to);
        }
        Piece promotion;
        if (flags & MoveFlag::BishopPromo) {
            promotion = {PieceType::Bishop, _turn};
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else if (flags & MoveFlag::RookPromo) {
            promotion = {PieceType::Rook, _turn};
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else if (flags & MoveFlag::KnightPromo) {
            promotion = {PieceType::Knight, _turn};
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else if (flags & MoveFlag::QueenPromo) {
            promotion = {PieceType::Queen, _turn};
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else {
            set_at(to, piece);
            state._hash ^= zobrist_bitstring(piece, to);
        }

        // Move rook if castling
        if (flags & MoveFlag::Castling) {
            int rankd = to - from;
            int dir = (rankd > 0) - (rankd < 0);
            Piece rook = {PieceType::Rook, _turn};
            uint64_t rook_rank = (_turn == Color::Black) ? rank8 : rank1;
            uint64_t rook_board =
                state._bitboards[rook.get_index()] & rook_rank;
            Square target(to - dir);
            if (rankd < 0) {
                rook_board &= fileA;
            } else {
                rook_board &= fileH;
            }
            Square initial_position = find_lsb(rook_board);
            clear_at(initial_position);
            set_at(target, rook);

            state._hash ^= zobrist_bitstring(rook, initial_position);
            state._hash ^= zobrist_bitstring(rook, target);
        }

        // Check for en passant capture
        if (flags & MoveFlag::EnPassant) {
            // Clear the square of the captured pawn
            int rankd = to - from;

            // One rank up or one rank down depending on current player
            int dir = (rankd > 0) - (rankd < 0);
            Square en_passant_square = state._en_passant_target - (dir * 8);
            Piece en_passant_piece = get_at(en_passant_square);
            clear_at(en_passant_square);

            state._hash ^=
                zobrist_bitstring(en_passant_piece, en_passant_square);
            state._hash ^= en_passant_bitstrings[state._en_passant_target % 8];
            state._en_passant_target = Squares::InvalidSquare;
        }

        // Update en passant position if pawn advanced two ranks
        if (!is_square_invalid(state._en_passant_target)) {
            state._hash ^= en_passant_bitstrings[state._en_passant_target % 8];
        }
        if (flags & MoveFlag::PawnDouble) {
            state._en_passant_target = from + (to - from) / 2;
            state._hash ^= en_passant_bitstrings[state._en_passant_target % 8];
        } else {
            state._en_passant_target = Squares::InvalidSquare;
        }

        // Reset halfmove counter if piece was pawn advance or move was a
        // capture
        if (flags & (MoveFlag::PawnAdvance | MoveFlag::PawnDouble |
                     MoveFlag::EnPassant | MoveFlag::Capture)) {
            state._halfmoves = 0;
        }

        // Update turn and fullmove counter
        if (_turn == Color::Black) {
            _fullmoves++;
        }
        _turn = static_cast<Color>(!_turn);
        state._hash ^= turn_bitstring;

        state._attackers = get_attackers();
        generate_moves();
    }

    bool Board::is_check() {
        Piece king = {PieceType::King, _turn};
        BoardState &state = _states[_current_state];
        uint64_t kingbit = state._bitboards[king.get_index()];
        return state._attackers & kingbit;
    }

    bool Board::is_draw() {
        // If there are only 2 pieces left (both kings), then it is a draw
        BoardState &state = _states[_current_state];
        uint64_t all_pieces = state._bitboards[12] | state._bitboards[13];
        int pieces_left = 0;
        while (all_pieces) {
            all_pieces &= (all_pieces - 1);
            pieces_left++;
        }
        return is_stalemate() || state._halfmoves >= 100 || pieces_left == 2;
    }

    Move
    Board::create_move(const Square from, const Square to, char promotion) {
        BoardState &state = _states[_current_state];
        for (auto &move : state._legal_moves) {
            Square move_from = move.get_from();
            Square move_to = move.get_to();
            MoveFlagSet flags = move.get_flags();

            if (move_from == from && move_to == to) {
                if (flags & (MoveFlag::RookPromo | MoveFlag::KnightPromo |
                             MoveFlag::BishopPromo | MoveFlag::QueenPromo)) {
                    switch (promotion) {
                    case 'r':
                        if (flags & MoveFlag::RookPromo) return move;
                        break;
                    case 'n':
                        if (flags & MoveFlag::KnightPromo) return move;
                        break;
                    case 'b':
                        if (flags & MoveFlag::BishopPromo) return move;
                        break;
                    case 'q':
                        if (flags & MoveFlag::QueenPromo) return move;
                        break;
                    default:
                        return {};
                        break;
                    }
                } else {
                    return move;
                }
            }
        }
        return {};
    }

    Move Board::create_move(std::string standard_notation) {
        BoardState &state = _states[_current_state];
        for (auto &move : state._legal_moves) {
            if (move.standard_notation() == standard_notation) {
                return move;
            }
        }
        return {};
    }

    void Board::print() {
        // Set code page to allow UTF16 characters to show (chcp 65001 on
        // powershell)
        if (_turn == Color::White) std::cout << "White's turn.\n";
        if (_turn == Color::Black) std::cout << "Black's turn.\n";
        std::string files = "ABCDEFGH";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << " ";
            for (int file = 0; file < 8; file++) {
                Piece piece = get_at_coords(rank, file);
                if (!piece.is_empty()) {
                    std::cout << piece.get_icon() << " ";
                } else {
                    std::cout << "- ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "  ";
        for (auto &f : files) {
            std::cout << f << " ";
        }
        std::cout << "\n";
    }
} // namespace brainiac
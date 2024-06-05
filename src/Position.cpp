#include "Position.hpp"

namespace Brainiac {
    Position::Position(std::string fen, Hasher hasher) {
        _states.reserve(256);
        _hasher = hasher;

        // Initialize the board
        std::vector<std::string> fields = tokenize(fen, ' ');
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
                while (PIECE_CHARS[char_idx] != c) {
                    char_idx++;
                }
                _board.set(Square(row * 8 + col),
                           Piece((char_idx / 6) * 6 + (char_idx % 6)));
                col++;
            }
        }

        // Castling state
        State &state = _states.emplace_back();
        state.castling = 0;
        for (auto &c : fields[2]) {
            if (c == 'K') state.castling |= (1 << CastlingRight::WK);
            else if (c == 'Q') state.castling |= (1 << CastlingRight::WQ);
            else if (c == 'k') state.castling |= (1 << CastlingRight::BK);
            else if (c == 'q') state.castling |= (1 << CastlingRight::BQ);
        }

        // EnPassant capture square
        state.ep_dst = fields[3].length() == 2 ? string_to_square(fields[3])
                                               : Square::Null;

        // Half-moves
        state.halfmoves = stoi(fields[4]);

        _fullmoves = stoi(fields[5]);

        // Current turn
        _turn = (fields[1][0] == 'w') ? Color::White : Color::Black;

        // Compute hash
        state.hash = hasher(_board, state.castling, _turn, state.ep_dst);

        generate_moves();
    }

    void Position::generate_moves() {
        State &state = _states[_states.size() - 1];

        Color op = static_cast<Color>(!_turn);

        Piece f_king = create_piece(PieceType::King, _turn);
        Piece f_pawn = create_piece(PieceType::Pawn, _turn);
        Piece f_rook = create_piece(PieceType::Rook, _turn);
        Piece f_knight = create_piece(PieceType::Knight, _turn);
        Piece f_bishop = create_piece(PieceType::Bishop, _turn);
        Piece f_queen = create_piece(PieceType::Queen, _turn);

        Piece o_king = create_piece(PieceType::King, op);
        Piece o_pawn = create_piece(PieceType::Pawn, op);
        Piece o_rook = create_piece(PieceType::Rook, op);
        Piece o_knight = create_piece(PieceType::Knight, op);
        Piece o_bishop = create_piece(PieceType::Bishop, op);
        Piece o_queen = create_piece(PieceType::Queen, op);

        MoveGen generator;
        generator.friends = _board.bitboard(_turn);
        generator.enemies = _board.bitboard(op);
        generator.all = generator.friends | generator.enemies;

        generator.f_king = _board.bitboard(f_king);
        generator.f_pawn = _board.bitboard(f_pawn);
        generator.f_rook = _board.bitboard(f_rook);
        generator.f_knight = _board.bitboard(f_knight);
        generator.f_bishop = _board.bitboard(f_bishop);
        generator.f_queen = _board.bitboard(f_queen);

        generator.o_king = _board.bitboard(o_king);
        generator.o_pawn = _board.bitboard(o_pawn);
        generator.o_rook = _board.bitboard(o_rook);
        generator.o_knight = _board.bitboard(o_knight);
        generator.o_bishop = _board.bitboard(o_bishop);
        generator.o_queen = _board.bitboard(o_queen);

        generator.ep_dst = state.ep_dst;
        generator.turn = _turn;
        generator.castling = state.castling;

        state.moves.clear();
        state.check = generator.generate(state.moves);
    }

    std::string Position::fen(bool include_counters) const {
        const State &state = _states[_states.size() - 1];
        std::string fen = "";
        for (int row = 7; row >= 0; row--) {
            int counter = 0;
            for (int col = 0; col < 8; col++) {
                Piece piece = _board.get(Square(row * 8 + col));
                if (piece != Piece::Empty) {
                    if (counter) {
                        fen += counter + '0';
                        counter = 0;
                    }
                    fen += PIECE_CHARS[piece];
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

        if (state.castling & (1 << CastlingRight::WK)) castling_rights += 'K';
        if (state.castling & (1 << CastlingRight::WQ)) castling_rights += 'Q';
        if (state.castling & (1 << CastlingRight::BK)) castling_rights += 'k';
        if (state.castling & (1 << CastlingRight::BQ)) castling_rights += 'q';
        if (castling_rights.length() == 0) castling_rights = "-";
        fen += " " + castling_rights;

        if (state.ep_dst == Square::Null) {
            fen += " -";
        } else {
            fen += " ";
            fen += square_to_string(state.ep_dst);
        }

        if (include_counters) {
            fen += " ";
            fen += std::to_string(state.halfmoves);
            fen += " ";
            fen += std::to_string(fullmoves());
        }
        return fen;
    }

    Hash Position::hash() const { return _states[_states.size() - 1].hash; }

    const Board &Position::board() const { return _board; }

    Color Position::turn() const { return _turn; }

    MoveList Position::moves() const {
        return _states[_states.size() - 1].moves;
    }

    const CastlingFlagSet Position::castling() const {
        return _states[_states.size() - 1].castling;
    }

    Clock Position::halfmoves() const {
        return _states[_states.size() - 1].halfmoves;
    }

    Clock Position::fullmoves() const { return _fullmoves; }

    bool Position::is_check() const {
        return _states[_states.size() - 1].check;
    }

    bool Position::is_checkmate() const {
        return is_check() && moves().size() == 0;
    }

    bool Position::is_stalemate() const {
        return !is_check() && moves().size() == 0;
    }

    bool Position::is_draw() const {
        const State &state = _states[_states.size() - 1];
        Bitboard black = _board.bitboard(Color::Black);
        Bitboard white = _board.bitboard(Color::White);
        unsigned rem = count_set_bitboard(white | black);
        return is_stalemate() || state.halfmoves >= 100 || rem == 2;
    }

    bool Position::is_start() const { return _states.size() == 1; }

    bool Position::is_quiet() {
        // Check if any moves will significantly affect evaluation
        for (Move move : moves()) {
            switch (move.type()) {
            case MoveType::Capture:
            case MoveType::QueenPromoCapture:
            case MoveType::KnightPromoCapture:
            case MoveType::BishopPromoCapture:
            case MoveType::RookPromoCapture:
            case MoveType::QueenPromo:
            case MoveType::KnightPromo:
            case MoveType::BishopPromo:
            case MoveType::RookPromo:
                return false;
            default:
                break;
            }
        }

        // Make sure current turn isn't in check
        return !is_check();
    }

    void Position::make(Move move) {
        State &state = _states.emplace_back(_states[_states.size() - 1]);
        state.halfmoves++;

        Square src_sq = move.src();
        Square dst_sq = move.dst();
        MoveType move_type = move.type();

        Piece src_piece = _board.get(src_sq);
        Piece dst_piece = _board.get(dst_sq);

        // Update previous move and captured piece
        state.prev_move = move;
        state.dst_piece = dst_piece;

        // Clear castling rights if relevant pieces were moved
        switch (src_piece) {
        case Piece::WhiteKing: {
            CastlingFlagSet king_side = (1 << CastlingRight::WK);
            CastlingFlagSet queen_side = (1 << CastlingRight::WQ);

            state.hash ^= _hasher.bitstring(state.castling);
            state.castling &= ~(king_side | queen_side);
            state.hash ^= _hasher.bitstring(state.castling);
            break;
        }
        case Piece::BlackKing: {
            CastlingFlagSet king_side = (1 << CastlingRight::BK);
            CastlingFlagSet queen_side = (1 << CastlingRight::BQ);

            state.hash ^= _hasher.bitstring(state.castling);
            state.castling &= ~(king_side | queen_side);
            state.hash ^= _hasher.bitstring(state.castling);
        }
        case Piece::WhiteRook: {
            Bitboard rook_mask = 1ULL << src_sq;

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[0]);
            CastlingFlagSet king_side = (1 << CastlingRight::WK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[0]);
            CastlingFlagSet queen_side = (1 << CastlingRight::WQ) & queen_mask;

            state.hash ^= _hasher.bitstring(state.castling);
            state.castling &= ~(king_side | queen_side);
            state.hash ^= _hasher.bitstring(state.castling);
            break;
        }
        case Piece::BlackRook: {
            Bitboard rook_mask = 1ULL << src_sq;

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[7]);
            CastlingFlagSet king_side = (1 << CastlingRight::BK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[7]);
            CastlingFlagSet queen_side = (1 << CastlingRight::BQ) & queen_mask;

            state.hash ^= _hasher.bitstring(state.castling);
            state.castling &= ~(king_side | queen_side);
            state.hash ^= _hasher.bitstring(state.castling);
            break;
        }
        default:
            break;
        }

        // Clear opponent castling rights if pieces were captured
        switch (dst_piece) {
        case Piece::WhiteRook: {
            Bitboard rook_mask = 1ULL << dst_sq;

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[0]);
            CastlingFlagSet king_side = (1 << CastlingRight::WK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[0]);
            CastlingFlagSet queen_side = (1 << CastlingRight::WQ) & queen_mask;

            state.hash ^= _hasher.bitstring(state.castling);
            state.castling &= ~(king_side | queen_side);
            state.hash ^= _hasher.bitstring(state.castling);
            break;
        }
        case Piece::BlackRook: {
            Bitboard rook_mask = 1ULL << dst_sq;

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[7]);
            CastlingFlagSet king_side = (1 << CastlingRight::BK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[7]);
            CastlingFlagSet queen_side = (1 << CastlingRight::BQ) & queen_mask;

            state.hash ^= _hasher.bitstring(state.castling);
            state.castling &= ~(king_side | queen_side);
            state.hash ^= _hasher.bitstring(state.castling);
            break;
        }
        default:
            break;
        }

        // Handle move type behaviors
        switch (move_type) {
        case MoveType::KnightPromo:
        case MoveType::KnightPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Knight, _turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::RookPromo:
        case MoveType::RookPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Rook, _turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::BishopPromo:
        case MoveType::BishopPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Bishop, _turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::QueenPromo:
        case MoveType::QueenPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Queen, _turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::PawnDouble:
            state.hash ^= _hasher.bitstring(state.ep_dst) &
                          -(state.ep_dst != Square::Null);
            state.ep_dst = static_cast<Square>(src_sq + (dst_sq - src_sq) / 2);
            state.hash ^= _hasher.bitstring(state.ep_dst);
            break;
        case MoveType::EnPassant: {
            // Map turn Color [0, 1] to Direction [1, -1]
            char pawn_dir = ((1 - _turn) * 2) - 1;

            state.ep_pawn = static_cast<Square>(state.ep_dst - (8 * pawn_dir));
            Piece target_piece = _board.get(state.ep_pawn);

            _board.clear(state.ep_pawn);

            state.hash ^= _hasher.bitstring(state.ep_pawn, target_piece);
            break;
        }

        case MoveType::KingCastle: {
            state.castle_rook_src = static_cast<Square>(_turn * 56 + 7);
            state.castle_rook_dst = static_cast<Square>(dst_sq - 1);
            Piece rook_piece = _board.get(state.castle_rook_src);

            _board.set(state.castle_rook_dst, rook_piece);
            _board.clear(state.castle_rook_src);

            state.hash ^= _hasher.bitstring(state.castle_rook_src, rook_piece);
            state.hash ^= _hasher.bitstring(state.castle_rook_dst, rook_piece);
            break;
        }
        case MoveType::QueenCastle: {
            state.castle_rook_src = static_cast<Square>(
                static_cast<std::underlying_type_t<Color>>(_turn) *
                static_cast<std::underlying_type_t<Square>>(Square::A8));
            state.castle_rook_dst = static_cast<Square>(dst_sq + 1);
            Piece rook_piece = _board.get(state.castle_rook_src);

            _board.set(state.castle_rook_dst, rook_piece);
            _board.clear(state.castle_rook_src);

            state.hash ^= _hasher.bitstring(state.castle_rook_src, rook_piece);
            state.hash ^= _hasher.bitstring(state.castle_rook_dst, rook_piece);
            break;
        }

        default:
            break;
        }

        // Clear the en passant square on all move types except pawn double
        switch (move_type) {
        case MoveType::PawnDouble:
            break;
        default:
            state.hash ^= _hasher.bitstring(state.ep_dst) &
                          -(state.ep_dst != Square::Null);
            state.ep_dst = Square::Null;
            break;
        }

        // Reset the halfmove clock for capture and pawn advance moves
        switch (move_type) {
        case MoveType::PawnDouble:
        case MoveType::EnPassant:
        case MoveType::Capture:
        case MoveType::KnightPromoCapture:
        case MoveType::RookPromoCapture:
        case MoveType::BishopPromoCapture:
        case MoveType::QueenPromoCapture:
            state.halfmoves = 0;
            break;
        case MoveType::Quiet: {
            bool keep = src_piece != BlackPawn && src_piece != WhitePawn;
            unsigned mask = -keep;
            state.halfmoves &= mask;
            break;
        }
        default:
            break;
        }

        // Move the src piece
        _board.set(dst_sq, src_piece);
        _board.clear(src_sq);

        bool non_empty = dst_piece != Piece::Empty;
        state.hash ^= _hasher.bitstring(src_sq, src_piece);
        state.hash ^= _hasher.bitstring(dst_sq, src_piece);
        state.hash ^= _hasher.bitstring(dst_sq, dst_piece) & -non_empty;

        // Update full move counter
        _fullmoves += (_turn == Color::Black);

        // Update turn
        _turn = static_cast<Color>(!_turn);
        state.hash ^= _hasher.bitstring(_turn);

        // Generate moves
        generate_moves();
    }

    void Position::undo() {
        // TODO: Revert the board ??
        // * If last move was king castle, move kingside rook back
        // * If last move was queen castle, move queenside rook back
        const State &state = _states[_states.size() - 1];

        // Update turn
        Color op = _turn;
        _turn = static_cast<Color>(!_turn);

        Square src_sq = state.prev_move.src();
        Square dst_sq = state.prev_move.dst();

        // Source piece is currently on the destination square
        Piece src_piece = _board.get(dst_sq);

        // Handle different move types
        switch (state.prev_move.type()) {
        case MoveType::Capture:
            _board.set(dst_sq, state.dst_piece);
            break;
        case MoveType::QueenPromoCapture:
        case MoveType::KnightPromoCapture:
        case MoveType::BishopPromoCapture:
        case MoveType::RookPromoCapture:
            src_piece = create_piece(PieceType::Pawn, _turn);
            _board.set(dst_sq, state.dst_piece);
            break;
        case MoveType::QueenPromo:
        case MoveType::KnightPromo:
        case MoveType::BishopPromo:
        case MoveType::RookPromo:
            src_piece = create_piece(PieceType::Pawn, _turn);
            _board.clear(dst_sq);
            break;
        case MoveType::EnPassant: {
            Piece pawn = create_piece(PieceType::Pawn, op);
            _board.set(state.ep_pawn, pawn);
            _board.clear(dst_sq);
            break;
        }
        case MoveType::KingCastle:
        case MoveType::QueenCastle: {
            Piece rook_piece = _board.get(state.castle_rook_dst);
            _board.set(state.castle_rook_src, rook_piece);
            _board.clear(state.castle_rook_dst);
            _board.clear(dst_sq);
            break;
        }
        case MoveType::Quiet:
        case MoveType::PawnDouble:
            _board.clear(dst_sq);
            break;
        case MoveType::Skip:
            // Early return on a skip move
            _states.pop_back();
            return;
        }

        // Move piece back to source square
        _board.set(src_sq, src_piece);

        // Revert state
        _states.pop_back();
    }

    void Position::skip() {
        State &state = _states.emplace_back(_states[_states.size() - 1]);
        state.halfmoves++;

        // Update full move counter
        _fullmoves += (_turn == Color::Black);

        // Update turn
        _turn = static_cast<Color>(!_turn);
        state.hash ^= _hasher.bitstring(_turn);
    }

    Move Position::find_move(const Square src,
                             const Square dst,
                             char promotion) const {
        for (const Move &move : moves()) {
            if (move.src() == src && move.dst() == dst) {
                if (promotion) {
                    switch (move.type()) {
                    case KnightPromo:
                    case KnightPromoCapture:
                        if (promotion == 'n') return move;
                        break;
                    case BishopPromo:
                    case BishopPromoCapture:
                        if (promotion == 'b') return move;
                        break;
                    case RookPromo:
                    case RookPromoCapture:
                        if (promotion == 'r') return move;
                        break;
                    case QueenPromo:
                    case QueenPromoCapture:
                        if (promotion == 'q') return move;
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

    Move Position::find_move(const std::string &str) const {
        if (str.length() < 4) return {};
        std::string from = str.substr(0, 2);
        std::string to = str.substr(2, 2);
        char promotion = 0;
        if (str.length() >= 5) {
            promotion = str[4];
        }
        return find_move(string_to_square(from),
                         string_to_square(to),
                         promotion);
    }

    void Position::print() const {
        if (_turn == Color::White) std::cout << "White's turn.\n";
        else std::cout << "Black's turn.\n";
        _board.print();
    }
} // namespace Brainiac
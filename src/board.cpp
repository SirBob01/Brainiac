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
                Piece piece(
                    static_cast<PieceType>(char_idx % PieceType::NPieces),
                    static_cast<Color>(char_idx / PieceType::NPieces));
                set_at(row * 8 + col, piece);
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
        generate_moves();

        // Calculate the Zobrist hash
        state._hash = zobrist_hash(_turn,
                                   state._bitboards,
                                   state._castling_rights,
                                   state._en_passant_target);
    }

    void Board::generate_moves() {
        // Clear move list
        BoardState &state = _states[_current_state];
        state._legal_moves.clear();

        // Relevant occupancy bitboards
        Color opp = static_cast<Color>(!_turn);
        Bitboard friends = get_bitboard(_turn);
        Bitboard enemies = get_bitboard(opp);
        Bitboard all = friends | enemies;

        // Current turn pieces
        Bitboard pawns = get_bitboard(PieceType::Pawn, _turn);
        Bitboard knights = get_bitboard(PieceType::Knight, _turn);
        Bitboard bishops = get_bitboard(PieceType::Bishop, _turn);
        Bitboard rooks = get_bitboard(PieceType::Rook, _turn);
        Bitboard queens = get_bitboard(PieceType::Queen, _turn);
        Bitboard king = get_bitboard(PieceType::King, _turn);

        // Opponent pieces
        Bitboard o_pawns = get_bitboard(PieceType::Pawn, opp);
        Bitboard o_knights = get_bitboard(PieceType::Knight, opp);
        Bitboard o_bishops = get_bitboard(PieceType::Bishop, opp);
        Bitboard o_rooks = get_bitboard(PieceType::Rook, opp);
        Bitboard o_queens = get_bitboard(PieceType::Queen, opp);
        Bitboard o_king = get_bitboard(PieceType::King, opp);

        // Opponent captures
        Bitboard o_pawns_caps = get_pawn_capture_mask(o_pawns, opp);
        Bitboard o_knights_caps = get_knight_mask(o_knights);
        Bitboard o_king_caps = get_king_mask(o_king);

        // Slider captures need to be handled per-axis
        Bitboard o_bishops_d1_caps =
            get_diagonal_mask(o_bishops, enemies, friends);
        Bitboard o_bishops_d2_caps =
            get_antidiag_mask(o_bishops, enemies, friends);

        Bitboard o_rooks_h_caps =
            get_horizontal_mask(o_rooks, enemies, friends);
        Bitboard o_rooks_v_caps = get_vertical_mask(o_rooks, enemies, friends);

        Bitboard o_queens_d1_caps =
            get_diagonal_mask(o_queens, enemies, friends);
        Bitboard o_queens_d2_caps =
            get_antidiag_mask(o_queens, enemies, friends);

        Bitboard o_queens_h_caps =
            get_horizontal_mask(o_queens, enemies, friends);
        Bitboard o_queens_v_caps =
            get_vertical_mask(o_queens, enemies, friends);

        Bitboard attackmask =
            ~enemies & (o_pawns_caps | o_knights_caps | o_bishops_d1_caps |
                        o_bishops_d2_caps | o_rooks_h_caps | o_rooks_v_caps |
                        o_queens_d1_caps | o_queens_d2_caps | o_queens_h_caps |
                        o_queens_v_caps | o_king_caps);

        // Calculate the check mask to filter out illegal moves
        Bitboard checkmask = 0xFFFFFFFFFFFFFFFF;
        if (attackmask & king) {
            state.check = true;
            checkmask =
                // Knight checkmask
                (get_knight_mask(king) & o_knights) |

                // Horizontal checkmask
                (get_horizontal_mask(king, friends, enemies) &
                 (o_rooks_h_caps | o_queens_h_caps | o_rooks | o_queens)) |

                // Vertical checkmask
                (get_vertical_mask(king, friends, enemies) &
                 (o_rooks_v_caps | o_queens_v_caps | o_rooks | o_queens)) |

                // Diagonal checkmask
                (get_diagonal_mask(king, friends, enemies) &
                 (o_bishops_d1_caps | o_queens_d1_caps | o_bishops |
                  o_queens)) |

                // Anti-diagonal checkmask
                (get_antidiag_mask(king, friends, enemies) &
                 (o_bishops_d2_caps | o_queens_d2_caps | o_bishops |
                  o_queens)) |

                // Pawn checkmask
                (get_pawn_capture_mask(king, _turn) & (o_pawns_caps | o_pawns));
        }

        // Filters
        // TODO: Calculate pinned pieces
        Bitboard check_filter = ~friends & checkmask;
        Bitboard attack_filter = ~friends & ~attackmask;

        // Generate pawn moves
        Bitboard bits;
        {
            bits = pawns;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                // Calculate move masks
                Bitboard pawn_single =
                    get_pawn_advance_mask(unit, all, _turn) & checkmask;
                Bitboard pawn_double =
                    get_pawn_double_mask(unit, all, _turn) & checkmask;
                Bitboard pawn_capture =
                    get_pawn_capture_mask(unit, _turn) & checkmask;

                // En-passant mask
                Bitboard pawn_ep = 0;
                if (!is_square_invalid(state._en_passant_target)) {
                    pawn_ep =
                        get_square_mask(state._en_passant_target) & checkmask;
                }

                // Single pawn advance
                while (pawn_single) {
                    Bitboard target = pawn_single & (-pawn_single);
                    if (target & end_ranks) {
                        for (int i = 0; i < 4; i++) {
                            Move move(square,
                                      find_lsb(target),
                                      pawn_single_flags | promotions[i]);
                            register_move(move);
                        }
                    } else {
                        Move move(square, find_lsb(target), pawn_single_flags);
                        register_move(move);
                    }
                    pawn_single &= (pawn_single - 1);
                }

                // Double pawn advance
                while (pawn_double) {
                    Bitboard target = pawn_double & (-pawn_double);
                    Move move(square, find_lsb(target), pawn_double_flags);
                    register_move(move);
                    pawn_double &= (pawn_double - 1);
                }

                // Regular captures
                Bitboard captures = pawn_capture & enemies;
                while (captures) {
                    Bitboard target = captures & (-captures);
                    if (target & end_ranks) {
                        for (int i = 0; i < 4; i++) {
                            Move move(square,
                                      find_lsb(target),
                                      pawn_capture_flags | promotions[i]);
                            register_move(move);
                        }
                    } else {
                        Move move(square, find_lsb(target), pawn_capture_flags);
                        register_move(move);
                    }
                    captures &= (captures - 1);
                }

                // En-passant captures
                Bitboard ep = pawn_capture & pawn_ep;
                while (ep) {
                    Bitboard target = ep & (-ep);
                    Move move(square, find_lsb(target), en_passant_flags);
                    register_move(move);
                    ep &= (ep - 1);
                }
                bits &= (bits - 1);
            }
        }

        // Knight moves
        {
            bits = knights;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                Bitboard knight_moves = get_knight_mask(unit) & check_filter;

                Bitboard captures = knight_moves & enemies;
                Bitboard quiet = knight_moves & ~enemies;
                while (quiet) {
                    Bitboard target = quiet & (-quiet);
                    Move move(square, find_lsb(target), 0);
                    register_move(move);
                    quiet &= (quiet - 1);
                }
                while (captures) {
                    Bitboard target = captures & (-captures);
                    Move move(square, find_lsb(target), MoveFlag::Capture);
                    register_move(move);
                    captures &= (captures - 1);
                }

                bits &= (bits - 1);
            }
        }

        // King moves
        {
            Bitboard unit = king;
            Square square = find_lsb(unit);

            Bitboard king_moves = get_king_mask(unit) & attack_filter;

            Bitboard captures = king_moves & enemies;
            Bitboard quiet = king_moves & ~enemies;
            while (quiet) {
                Bitboard target = quiet & (-quiet);
                Move move(square, find_lsb(target), 0);
                register_move(move);
                quiet &= (quiet - 1);
            }
            while (captures) {
                Bitboard target = captures & (-captures);
                Move move(square, find_lsb(target), MoveFlag::Capture);
                register_move(move);
                captures &= (captures - 1);
            }

            // TODO: Optimize?
            CastlingFlagSet rights =
                state._castling_rights & color_castling_rights[_turn];
            while (rights) {
                Castle side = static_cast<Castle>(rights & (-rights));

                // King cannot move in range of his attackers
                Bitboard mask = get_castling_mask(all, side) & ~attackmask;
                if (mask) {
                    Move move(square, find_lsb(mask), MoveFlag::Castling);
                    register_move(move);
                }
                rights &= (rights - 1);
            }
        }

        // Bishop moves
        {
            bits = bishops;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                Bitboard moves =
                    get_bishop_mask(unit, friends, enemies) & checkmask;

                Bitboard captures = moves & enemies;
                Bitboard quiet = moves & ~enemies;
                while (quiet) {
                    Bitboard target = quiet & (-quiet);
                    Move move(square, find_lsb(target), 0);
                    register_move(move);
                    quiet &= (quiet - 1);
                }
                while (captures) {
                    Bitboard target = captures & (-captures);
                    Move move(square, find_lsb(target), MoveFlag::Capture);
                    register_move(move);
                    captures &= (captures - 1);
                }
                bits &= (bits - 1);
            }
        }

        // Rook moves
        {
            bits = rooks;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                Bitboard moves =
                    get_rook_mask(unit, friends, enemies) & checkmask;

                Bitboard captures = moves & enemies;
                Bitboard quiet = moves & ~enemies;
                while (quiet) {
                    Bitboard target = quiet & (-quiet);
                    Move move(square, find_lsb(target), 0);
                    register_move(move);
                    quiet &= (quiet - 1);
                }
                while (captures) {
                    Bitboard target = captures & (-captures);
                    Move move(square, find_lsb(target), MoveFlag::Capture);
                    register_move(move);
                    captures &= (captures - 1);
                }
                bits &= (bits - 1);
            }
        }

        // Queen moves
        {
            bits = queens;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                Bitboard moves =
                    get_queen_mask(unit, friends, enemies) & checkmask;

                Bitboard captures = moves & enemies;
                Bitboard quiet = moves & ~enemies;
                while (quiet) {
                    Bitboard target = quiet & (-quiet);
                    Move move(square, find_lsb(target), 0);
                    register_move(move);
                    quiet &= (quiet - 1);
                }
                while (captures) {
                    Bitboard target = captures & (-captures);
                    Move move(square, find_lsb(target), MoveFlag::Capture);
                    register_move(move);
                    captures &= (captures - 1);
                }
                bits &= (bits - 1);
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
        Bitboard mask = get_square_mask(sq);
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
        Bitboard mask = get_square_mask(sq);

        BoardState &state = _states[_current_state];
        state._bitboards[PieceType::NPieces2 + piece.color] |= mask;
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
        Bitboard mask = ~(get_square_mask(sq));
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
        generate_moves();
    }

    void Board::make_move(const Move &move) {
        BoardState &state = push_state();

        Square from = move.get_from();
        Square to = move.get_to();
        MoveFlagSet flags = move.get_flags();

        Piece piece = get_at(from);
        Piece target = get_at(to);

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
                Bitboard mask = get_square_mask(from);
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
            Bitboard mask = get_square_mask(to);
            Bitboard rank = (_turn == Color::White) ? rank8 : rank1;
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
            Piece rook(PieceType::Rook, _turn);
            Bitboard rook_rank = (_turn == Color::Black) ? rank8 : rank1;
            Bitboard rook_board =
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
        generate_moves();
    }

    bool Board::is_draw() {
        // If there are only 2 pieces left (both kings), then it is a draw
        BoardState &state = _states[_current_state];
        Bitboard all_pieces = state._bitboards[12] | state._bitboards[13];
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
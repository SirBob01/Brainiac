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
        state._check = false;
        state._legal_moves.clear();

        // Relevant occupancy bitboards
        Color opp = static_cast<Color>(!_turn);
        Bitboard friends = get_bitboard(_turn);
        Bitboard enemies = get_bitboard(opp);
        Bitboard all = friends | enemies;
        int pawn_dir = _turn == Color::White ? 1 : -1;

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

        Bitboard o_bishops_or_queens = o_bishops | o_queens;
        Bitboard o_rooks_or_queens = o_rooks | o_queens;
        Bitboard all_no_king = (friends & ~king) | enemies;

        // Opponent moves
        Bitboard o_pawns_moves = get_pawn_capture_mask(o_pawns, opp);
        Bitboard o_knights_moves = get_knight_mask(o_knights);
        Bitboard o_king_moves = get_king_mask(o_king);

        // Slider moves need to be handled per-axis
        Bitboard cardinal_masks[4]; // { n, s, e, w }
        Bitboard ordinal_masks[4];  // { ne, se, sw, nw }
        Bitboard cardinal_queens_masks[4];
        Bitboard ordinal_queens_masks[4];

        get_cardinal_masks(o_rooks, enemies, friends, cardinal_masks);
        get_ordinal_masks(o_bishops, enemies, friends, ordinal_masks);
        get_cardinal_masks(o_queens, enemies, friends, cardinal_queens_masks);
        get_ordinal_masks(o_queens, enemies, friends, ordinal_queens_masks);

        Bitboard o_rooks_v_moves = cardinal_masks[0] | cardinal_masks[1];
        Bitboard o_rooks_h_moves = cardinal_masks[2] | cardinal_masks[3];

        Bitboard o_bishops_d1_moves = ordinal_masks[0] | ordinal_masks[2];
        Bitboard o_bishops_d2_moves = ordinal_masks[1] | ordinal_masks[3];

        Bitboard o_queens_v_moves =
            cardinal_queens_masks[0] | cardinal_queens_masks[1];
        Bitboard o_queens_h_moves =
            cardinal_queens_masks[2] | cardinal_queens_masks[3];
        Bitboard o_queens_d1_moves =
            ordinal_queens_masks[0] | ordinal_queens_masks[2];
        Bitboard o_queens_d2_moves =
            ordinal_queens_masks[1] | ordinal_queens_masks[3];

        // Slider captures ignoring the king
        Bitboard o_bishops_king_danger =
            get_bishop_mask(o_bishops, 0, all_no_king);
        Bitboard o_rooks_king_danger = get_rook_mask(o_rooks, 0, all_no_king);
        Bitboard o_queens_d1_king_danger =
            get_queen_mask(o_queens, 0, all_no_king);

        // Squares that are attacked by the enemy
        Bitboard attackmask =
            o_pawns_moves | o_knights_moves | o_bishops_d1_moves |
            o_bishops_d2_moves | o_rooks_h_moves | o_rooks_v_moves |
            o_queens_d1_moves | o_queens_d2_moves | o_queens_h_moves |
            o_queens_v_moves | o_king_moves;

        // Squares that king cannot go to
        Bitboard king_dangermask = o_pawns_moves | o_knights_moves |
                                   o_bishops_king_danger | o_rooks_king_danger |
                                   o_queens_d1_king_danger | o_king_moves;

        // Calculate the check mask to filter out illegal moves
        Bitboard checkmask = 0xFFFFFFFFFFFFFFFF;
        if (attackmask & king) {
            state._check = true;

            get_cardinal_masks(king, friends, enemies, cardinal_masks);
            get_ordinal_masks(king, friends, enemies, ordinal_masks);
            checkmask =
                // Knight checkmask
                (get_knight_mask(king) & o_knights) |

                // Vertical checkmask
                ((cardinal_masks[0] | cardinal_masks[1]) &
                 (o_rooks_v_moves | o_queens_v_moves | o_rooks_or_queens)) |

                // Horizontal checkmask
                ((cardinal_masks[2] | cardinal_masks[3]) &
                 (o_rooks_h_moves | o_queens_h_moves | o_rooks_or_queens)) |

                // Diagonal checkmask
                ((ordinal_masks[0] | ordinal_masks[2]) &
                 (o_bishops_d1_moves | o_queens_d1_moves |
                  o_bishops_or_queens)) |

                // Anti-diagonal checkmask
                ((ordinal_masks[1] | ordinal_masks[3]) &
                 (o_bishops_d2_moves | o_queens_d2_moves |
                  o_bishops_or_queens)) |

                // Pawn checkmask
                (get_pawn_capture_mask(king, _turn) & o_pawns);
        }

        // Calculate pin masks for each slider
        get_cardinal_masks(o_rooks, king, 0, cardinal_masks);
        get_ordinal_masks(o_bishops, king, 0, ordinal_masks);
        get_cardinal_masks(o_queens, king, 0, cardinal_queens_masks);
        get_ordinal_masks(o_queens, king, 0, ordinal_queens_masks);

        Bitboard o_rooks_v_clear =
            cardinal_masks[0] | cardinal_masks[1] | o_rooks;
        Bitboard o_rooks_h_clear =
            cardinal_masks[2] | cardinal_masks[3] | o_rooks;

        Bitboard o_bishops_d1_clear =
            ordinal_masks[0] | ordinal_masks[2] | o_bishops;
        Bitboard o_bishops_d2_clear =
            ordinal_masks[1] | ordinal_masks[3] | o_bishops;

        Bitboard o_queens_v_clear =
            cardinal_queens_masks[0] | cardinal_queens_masks[1] | o_queens;
        Bitboard o_queens_h_clear =
            cardinal_queens_masks[2] | cardinal_queens_masks[3] | o_queens;
        Bitboard o_queens_d1_clear =
            ordinal_queens_masks[0] | ordinal_queens_masks[2] | o_queens;
        Bitboard o_queens_d2_clear =
            ordinal_queens_masks[1] | ordinal_queens_masks[3] | o_queens;

        get_cardinal_masks(king, 0, o_rooks, cardinal_masks);
        get_ordinal_masks(king, 0, o_bishops, ordinal_masks);
        get_cardinal_masks(king, 0, o_queens, cardinal_queens_masks);
        get_ordinal_masks(king, 0, o_queens, ordinal_queens_masks);

        Bitboard rook_pins[4] = {
            // N
            (cardinal_masks[0] & o_rooks_v_clear),
            // S
            (cardinal_masks[1] & o_rooks_v_clear),
            // E
            (cardinal_masks[2] & o_rooks_h_clear),
            // W
            (cardinal_masks[3] & o_rooks_h_clear),
        };
        Bitboard bishop_pins[4] = {
            // NE
            (ordinal_masks[0] & o_bishops_d1_clear),
            // SW
            (ordinal_masks[2] & o_bishops_d1_clear),
            // NW
            (ordinal_masks[3] & o_bishops_d2_clear),
            // SE
            (ordinal_masks[1] & o_bishops_d2_clear),
        };
        Bitboard queen_pins[8] = {
            // N
            (cardinal_queens_masks[0] & o_queens_v_clear),
            // S
            (cardinal_queens_masks[1] & o_queens_v_clear),
            // E
            (cardinal_queens_masks[2] & o_queens_h_clear),
            // W
            (cardinal_queens_masks[3] & o_queens_h_clear),
            // NE
            (ordinal_queens_masks[0] & o_queens_d1_clear),
            // SW
            (ordinal_queens_masks[2] & o_queens_d1_clear),
            // NW
            (ordinal_queens_masks[3] & o_queens_d2_clear),
            // SE
            (ordinal_queens_masks[1] & o_queens_d2_clear),
        };
        for (int i = 0; i < 4; i++) {
            Bitboard mask_friends = rook_pins[i] & friends;
            Bitboard mask_enemies = rook_pins[i] & enemies;
            if ((mask_enemies & (mask_enemies - 1)) ||
                (mask_friends & (mask_friends - 1))) {
                rook_pins[i] = 0;
            }
        }
        for (int i = 0; i < 4; i++) {
            Bitboard mask_friends = bishop_pins[i] & friends;
            Bitboard mask_enemies = bishop_pins[i] & enemies;
            if ((mask_enemies & (mask_enemies - 1)) ||
                (mask_friends & (mask_friends - 1))) {
                bishop_pins[i] = 0;
            }
        }
        for (int i = 0; i < 8; i++) {
            Bitboard mask_friends = queen_pins[i] & friends;
            Bitboard mask_enemies = queen_pins[i] & enemies;
            if ((mask_enemies & (mask_enemies - 1)) ||
                (mask_friends & (mask_friends - 1))) {
                queen_pins[i] = 0;
            }
        }
        Bitboard v_pins =
            (rook_pins[0] | rook_pins[1] | queen_pins[0] | queen_pins[1]);
        Bitboard h_pins =
            (rook_pins[2] | rook_pins[3] | queen_pins[2] | queen_pins[3]);
        Bitboard d1_pins =
            (bishop_pins[0] | bishop_pins[1] | queen_pins[4] | queen_pins[5]);
        Bitboard d2_pins =
            (bishop_pins[2] | bishop_pins[3] | queen_pins[6] | queen_pins[7]);
        Bitboard hv_pins = h_pins | v_pins;
        Bitboard d12_pins = d1_pins | d2_pins;
        Bitboard all_pins = hv_pins | d12_pins;

        // Filters
        Bitboard knight_filter = ~friends & checkmask;
        Bitboard king_filter = ~friends & ~king_dangermask;

        // King moves
        {
            Bitboard unit = king;
            Square square = find_lsb(unit);

            Bitboard king_moves = get_king_mask(unit) & king_filter;

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

            // Castling moves
            if (!state._check) {
                CastlingFlagSet rights =
                    state._castling_rights & (castling_rights[_turn * 2] |
                                              castling_rights[_turn * 2 + 1]);
                while (rights) {
                    Castle side = static_cast<Castle>(rights & (-rights));

                    // King cannot move in range of his attackers
                    Bitboard mask = get_castling_mask(all, side) & ~attackmask;

                    if (mask) {
                        Square to = find_lsb(mask);
                        int rankd = to - square;
                        int dir = (rankd > 0) - (rankd < 0);
                        Square pass_through = to - dir;
                        if (!(get_square_mask(pass_through) & attackmask)) {
                            Move move(square, to, MoveFlag::Castling);
                            register_move(move);
                        }
                    }
                    rights &= (rights - 1);
                }
            }
        }

        // Early return on double check, only king can move
        if (state._check && count_set_bits(checkmask & enemies) > 1) {
            return;
        }

        // Pawn moves
        Bitboard bits;
        {
            bits = pawns;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                // En-passant mask (captured piece cannot be pinned)
                Bitboard pawn_ep = 0;
                if (!is_square_invalid(state._en_passant_target)) {
                    Bitboard captured_pawn = get_square_mask(
                        state._en_passant_target - (pawn_dir * 8));
                    bool horizontal_discovered =
                        get_horizontal_mask(king,
                                            friends & ~unit,
                                            enemies & ~captured_pawn) &
                        (o_queens | o_rooks);
                    if (!(captured_pawn & all_pins) && !horizontal_discovered) {
                        pawn_ep = get_square_mask(state._en_passant_target);
                    }
                }

                // Calculate move masks
                Bitboard pawn_single =
                    get_pawn_advance_mask(unit, all, _turn) & checkmask;
                Bitboard pawn_double =
                    get_pawn_double_mask(unit, all, _turn) & checkmask;
                Bitboard pawn_capture = (get_pawn_capture_mask(unit, _turn) &
                                         (checkmask | pawn_ep));
                if (unit & all_pins) {
                    pawn_single &= v_pins;
                    pawn_double &= v_pins;
                    pawn_capture &= d12_pins;
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
            bits = knights & ~all_pins; // Pinned knights can never move
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                Bitboard knight_moves = get_knight_mask(unit) & knight_filter;

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

        // Bishop moves
        {
            bits = bishops;
            while (bits) {
                Bitboard unit = (bits & -bits);
                Square square = find_lsb(unit);

                Bitboard moves =
                    get_bishop_mask(unit, friends, enemies) & checkmask;
                if (unit & h_pins) {
                    moves &= h_pins;
                }
                if (unit & v_pins) {
                    moves &= v_pins;
                }
                if (unit & d1_pins) {
                    moves &= d1_pins;
                }
                if (unit & d2_pins) {
                    moves &= d2_pins;
                }

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
                if (unit & h_pins) {
                    moves &= h_pins;
                }
                if (unit & v_pins) {
                    moves &= v_pins;
                }
                if (unit & d1_pins) {
                    moves &= d1_pins;
                }
                if (unit & d2_pins) {
                    moves &= d2_pins;
                }

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
                if (unit & h_pins) {
                    moves &= h_pins;
                }
                if (unit & v_pins) {
                    moves &= v_pins;
                }
                if (unit & d1_pins) {
                    moves &= d1_pins;
                }
                if (unit & d2_pins) {
                    moves &= d2_pins;
                }

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

    void Board::skip_move() {
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
        Color opp = static_cast<Color>(!_turn);

        Square from = move.get_from();
        Square to = move.get_to();
        MoveFlagSet flags = move.get_flags();

        Piece piece = get_at(from);
        Piece target = get_at(to);

        // Unset castling flags if relevant pieces were moved
        Castle k_castle = castling_rights[2 * _turn];
        Castle q_castle = castling_rights[2 * _turn + 1];
        Castle o_k_castle = castling_rights[2 * opp];
        Castle o_q_castle = castling_rights[2 * opp + 1];

        CastlingFlagSet castle_flags = k_castle | q_castle;
        if (state._castling_rights & castle_flags) {
            if (piece.type == PieceType::King) {
                if (state._castling_rights & k_castle) {
                    state._hash ^= castling_bitstrings[find_lsb(k_castle)];
                }
                if (state._castling_rights & q_castle) {
                    state._hash ^= castling_bitstrings[find_lsb(q_castle)];
                }
                state._castling_rights &= ~castle_flags;
            } else if (piece.type == PieceType::Rook) {
                Bitboard mask = get_square_mask(from);
                Bitboard rank = castling_ranks[_turn];
                if (mask & fileA & rank) {
                    if (state._castling_rights & q_castle) {
                        state._hash ^= castling_bitstrings[find_lsb(q_castle)];
                    }
                    state._castling_rights &= ~q_castle;
                } else if (mask & fileH & rank) {
                    if (state._castling_rights & k_castle) {
                        state._hash ^= castling_bitstrings[find_lsb(k_castle)];
                    }
                    state._castling_rights &= ~k_castle;
                }
            }
        }

        // Unset castling opponent flags if pieces were captured
        if (target.type == PieceType::Rook) {
            Bitboard mask = get_square_mask(to);
            Bitboard rank = castling_ranks[opp];
            if (mask & fileA & rank) {
                if (state._castling_rights & o_q_castle) {
                    state._hash ^= castling_bitstrings[find_lsb(o_q_castle)];
                }
                state._castling_rights &= ~o_q_castle;
            } else if (mask & fileH & rank) {
                if (state._castling_rights & o_k_castle) {
                    state._hash ^= castling_bitstrings[find_lsb(o_k_castle)];
                }
                state._castling_rights &= ~o_k_castle;
            }
        }

        // Move to target square and handle promotions
        clear_at(from, piece);
        state._hash ^= zobrist_bitstring(piece, from);
        if (!target.is_empty()) {
            clear_at(to, target);
            state._hash ^= zobrist_bitstring(target, to);
        }
        if (flags & MoveFlag::BishopPromo) {
            const Piece promotion(PieceType::Bishop, _turn);
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else if (flags & MoveFlag::RookPromo) {
            const Piece promotion(PieceType::Rook, _turn);
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else if (flags & MoveFlag::KnightPromo) {
            const Piece promotion(PieceType::Knight, _turn);
            set_at(to, promotion);
            state._hash ^= zobrist_bitstring(promotion, to);
        } else if (flags & MoveFlag::QueenPromo) {
            const Piece promotion(PieceType::Queen, _turn);
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
            const Piece rook(PieceType::Rook, _turn);
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
            clear_at(initial_position, rook);
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
            clear_at(en_passant_square, en_passant_piece);

            state._hash ^=
                zobrist_bitstring(en_passant_piece, en_passant_square);
            state._hash ^= en_passant_bitstrings[state._en_passant_target % 8];
            state._en_passant_target = Squares::InvalidSquare;

            if (!is_square_invalid(state._en_passant_target)) {
                state._hash ^=
                    en_passant_bitstrings[state._en_passant_target % 8];
            }
        }

        // Update en passant position if pawn advanced two ranks
        if (flags & MoveFlag::PawnDouble) {
            state._en_passant_target = from + (to - from) / 2;
            state._hash ^= en_passant_bitstrings[state._en_passant_target % 8];
        } else {
            state._en_passant_target = Squares::InvalidSquare;
        }

        // Reset halfmoves if move was a pawn advance or a capture
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
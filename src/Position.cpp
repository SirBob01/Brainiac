#include "Position.hpp"
#include "Square.hpp"

namespace Brainiac {
    Position::Position(std::string fen, Hasher hasher) {
        _states.reserve(128);
        _states.emplace_back(fen, hasher);
        _index = 0;
        _hasher = hasher;
    }

    State &Position::push_state() {
        _states.resize(_index + 1);
        _states.emplace_back(_states[_index]);
        _index++;

        _states[_index].halfmoves++;
        return _states[_index];
    }

    std::string Position::fen(bool include_counters) const {
        return _states[_index].fen(include_counters);
    }

    Hash Position::hash() const { return _states[_index].hash; }

    const Board &Position::board() const { return _states[_index].board; }

    Color Position::turn() const { return _states[_index].turn; }

    const MoveList &Position::moves() const { return _states[_index].moves; }

    const CastlingFlagSet Position::castling() const {
        return _states[_index].castling;
    }

    Clock Position::halfmoves() const { return _states[_index].halfmoves; }

    Clock Position::fullmoves() const { return _states[_index].fullmoves; }

    bool Position::is_check() const { return _states[_index].check; }

    bool Position::is_checkmate() const {
        return is_check() && moves().size() == 0;
    }

    bool Position::is_stalemate() const {
        return !is_check() && moves().size() == 0;
    }

    bool Position::is_draw() const {
        const State &state = _states[_index];
        Bitboard all = state.board.bitboard(Color::Black) |
                       state.board.bitboard(Color::White);
        unsigned rem = count_set_bitboard(all);
        return is_stalemate() || state.halfmoves >= 100 || rem == 2;
    }

    bool Position::is_start() const { return _index == 0; }

    bool Position::is_end() const { return _index == _states.size() - 1; }

    bool Position::is_quiet() {
        // Check turn's moves
        for (Move move : moves()) {
            if (move.type() != MoveType::Quiet) {
                return false;
            }
        }
        bool turn_check = is_check();

        // Check opponent's moves
        skip();
        for (Move move : moves()) {
            if (move.type() != MoveType::Quiet) {
                undo();
                return false;
            }
        }
        bool opp_check = is_check();
        undo();

        // Make sure board isn't in check
        return !turn_check && !opp_check;
    }

    void Position::make(Move move) {
        State &state = push_state();

        Square src_sq = move.src();
        Square dst_sq = move.dst();
        MoveType move_type = move.type();

        Piece src_piece = state.board.get(src_sq);
        Piece dst_piece = state.board.get(dst_sq);

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
            src_piece = create_piece(PieceType::Knight, state.turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::RookPromo:
        case MoveType::RookPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Rook, state.turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::BishopPromo:
        case MoveType::BishopPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Bishop, state.turn);
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            break;
        case MoveType::QueenPromo:
        case MoveType::QueenPromoCapture:
            state.hash ^= _hasher.bitstring(src_sq, src_piece);
            src_piece = create_piece(PieceType::Queen, state.turn);
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
            int pawn_dir = ((1 - state.turn) * 2) - 1;

            Square target = static_cast<Square>(state.ep_dst - (8 * pawn_dir));
            Piece target_piece = state.board.get(target);

            state.board.clear(target);

            state.hash ^= _hasher.bitstring(target, target_piece);
            break;
        }

        case MoveType::KingCastle: {
            Square rook_sq = static_cast<Square>(state.turn * 56 + 7);
            Square rook_dst_sq = static_cast<Square>(dst_sq - 1);
            Piece rook_piece = state.board.get(rook_sq);

            state.board.set(rook_dst_sq, rook_piece);
            state.board.clear(rook_sq);

            state.hash ^= _hasher.bitstring(rook_sq, rook_piece);
            state.hash ^= _hasher.bitstring(rook_dst_sq, rook_piece);
            break;
        }
        case MoveType::QueenCastle: {
            Square rook_sq = static_cast<Square>(
                static_cast<std::underlying_type_t<Color>>(state.turn) *
                static_cast<std::underlying_type_t<Square>>(Square::A8));
            Square rook_dst_sq = static_cast<Square>(dst_sq + 1);
            Piece rook_piece = state.board.get(rook_sq);

            state.board.set(rook_dst_sq, rook_piece);
            state.board.clear(rook_sq);

            state.hash ^= _hasher.bitstring(rook_sq, rook_piece);
            state.hash ^= _hasher.bitstring(rook_dst_sq, rook_piece);
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
        state.board.set(dst_sq, src_piece);
        state.board.clear(src_sq);

        state.hash ^= _hasher.bitstring(src_sq, src_piece);
        state.hash ^= _hasher.bitstring(dst_sq, src_piece);
        state.hash ^=
            _hasher.bitstring(dst_sq, dst_piece) & -(dst_piece != Piece::Empty);

        // Update turn and fullmove counter
        state.fullmoves += (state.turn == Color::Black);
        state.turn = static_cast<Color>(!state.turn);
        state.hash ^= _hasher.bitstring(state.turn);

        state.generate_moves();
    }

    void Position::undo() { _index--; }

    void Position::redo() { _index++; }

    void Position::skip() {
        State &state = push_state();

        // Update state
        state.fullmoves += (state.turn == Color::Black);
        state.turn = static_cast<Color>(!state.turn);
        state.hash ^= _hasher.bitstring(state.turn);

        state.generate_moves();
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

    void Position::print() const { _states[_index].print(); }
} // namespace Brainiac
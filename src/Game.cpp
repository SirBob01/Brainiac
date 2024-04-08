#include "Game.hpp"

namespace Brainiac {
    Game::Game(std::string fen, Hasher hasher) {
        _states.reserve(64);
        _states.emplace_back(fen, hasher);
        _index = 0;
        _hasher = hasher;
    }

    State &Game::push_state() {
        _states.resize(_index + 1);
        _states.emplace_back(_states[_index]);
        _index++;

        _states[_index].halfmoves++;
        return _states[_index];
    }

    std::string Game::fen() const { return _states[_index].fen(); }

    Hash Game::hash() const { return _states[_index].hash; }

    const MoveList &Game::moves() const { return _states[_index].moves; }

    bool Game::is_check() const { return _states[_index].check; }

    bool Game::is_checkmate() const {
        return is_check() && moves().size() == 0;
    }

    bool Game::is_statelmate() const {
        return !is_check() && moves().size() == 0;
    }

    void Game::make(Move move) {
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
            state.castling &= ~(king_side | queen_side);
            break;
        }
        case Piece::BlackKing: {
            CastlingFlagSet king_side = (1 << CastlingRight::BK);
            CastlingFlagSet queen_side = (1 << CastlingRight::BQ);
            state.castling &= ~(king_side | queen_side);
        }
        case Piece::WhiteRook: {
            Bitboard rook_mask = get_square_mask(src_sq);

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[0]);
            CastlingFlagSet king_side = (1 << CastlingRight::WK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[0]);
            CastlingFlagSet queen_side = (1 << CastlingRight::WQ) & queen_mask;

            state.castling &= ~(king_side | queen_side);
            break;
        }
        case Piece::BlackRook: {
            Bitboard rook_mask = get_square_mask(src_sq);

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[7]);
            CastlingFlagSet king_side = (1 << CastlingRight::BK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[7]);
            CastlingFlagSet queen_side = (1 << CastlingRight::BQ) & queen_mask;

            state.castling &= ~(king_side | queen_side);
            break;
        }
        default:
            break;
        }

        // Clear opponent castling rights if pieces were captured
        switch (dst_piece) {
        case Piece::WhiteRook: {
            Bitboard rook_mask = get_square_mask(src_sq);

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[0]);
            CastlingFlagSet king_side = (1 << CastlingRight::WK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[0]);
            CastlingFlagSet queen_side = (1 << CastlingRight::WQ) & queen_mask;

            state.castling &= ~(king_side | queen_side);
            break;
        }
        case Piece::BlackRook: {
            Bitboard rook_mask = get_square_mask(src_sq);

            uint8_t king_mask =
                -static_cast<bool>(rook_mask & FILES[7] & RANKS[7]);
            CastlingFlagSet king_side = (1 << CastlingRight::BK) & king_mask;

            uint8_t queen_mask =
                -static_cast<bool>(rook_mask & FILES[0] & RANKS[7]);
            CastlingFlagSet queen_side = (1 << CastlingRight::BQ) & queen_mask;

            state.castling &= ~(king_side | queen_side);
            break;
        }
        default:
            break;
        }

        // Handle move type behaviors
        switch (move_type) {
        case MoveType::KnightPromo:
        case MoveType::KnightPromoCapture:
            src_piece = create_piece(PieceType::Knight, state.turn);
            break;
        case MoveType::RookPromo:
        case MoveType::RookPromoCapture:
            src_piece = create_piece(PieceType::Rook, state.turn);
            break;
        case MoveType::BishopPromo:
        case MoveType::BishopPromoCapture:
            src_piece = create_piece(PieceType::Bishop, state.turn);
            break;
        case MoveType::QueenPromo:
        case MoveType::QueenPromoCapture:
            src_piece = create_piece(PieceType::Queen, state.turn);
            break;
        case MoveType::PawnDouble:
            state.ep_dst = static_cast<Square>(src_sq + (dst_sq - src_sq) / 2);
            break;
        case MoveType::EnPassant: {
            // Map turn Color [0, 1] to Direction [1, -1]
            int pawn_dir = ((1 - state.turn) * 2) - 1;
            Square target = static_cast<Square>(state.ep_dst - (8 * pawn_dir));
            state.board.clear(target);
            break;
        }

        case MoveType::KingCastle: {
            Square rook_sq = static_cast<Square>(state.turn * 56 + 7);
            Square rook_dst_sq = static_cast<Square>(dst_sq - 1);
            state.board.set(rook_dst_sq, state.board.get(rook_sq));
            state.board.clear(rook_sq);
            break;
        }
        case MoveType::QueenCastle: {
            Square rook_sq = static_cast<Square>(state.turn * Square::A8);
            Square rook_dst_sq = static_cast<Square>(dst_sq + 1);
            state.board.set(rook_dst_sq, state.board.get(rook_sq));
            state.board.clear(rook_sq);
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

        // Update turn and fullmove counter
        state.fullmoves += (state.turn == Color::Black);
        state.turn = static_cast<Color>(!state.turn);
        state.hash ^= _hasher.bitstring(state.turn);
        state.generate_moves();
    }

    void Game::undo() { _index--; }

    void Game::skip() {
        State &state = push_state();

        // Update state
        state.fullmoves += (state.turn == Color::Black);
        state.turn = static_cast<Color>(!state.turn);
        state.hash ^= _hasher.bitstring(state.turn);
        state.generate_moves();
    }

    void Game::print() const { _states[_index].print(); }
} // namespace Brainiac
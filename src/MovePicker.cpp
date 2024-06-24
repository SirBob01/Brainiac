#include "MovePicker.hpp"

namespace Brainiac {
    MoveValue MovePicker::see_target(Square target, Position &position) {
        const Board &board = position.board();
        const MoveList &moves = position.moves();

        Piece victim = board.get(target);
        if (victim == Piece::Empty) {
            return 0;
        }

        Value value = 0;
        Value mvv = std::abs(PIECE_WEIGHTS[victim]);
        Value lva = MAX_VALUE;

        Move best_move = moves[0];
        for (Move move : moves) {
            if (move.dst() != target) continue;

            Piece attacker = board.get(move.src());
            Value attacker_value = std::abs(PIECE_WEIGHTS[attacker]);
            if (attacker_value < lva) {
                lva = attacker_value;
                best_move = move;
            }
        }

        if (lva != MAX_VALUE) {
            position.make(best_move);
            value = std::max(0, mvv - see_target(target, position));
            position.undo();
        }

        return value;
    }

    MoveValue MovePicker::evaluate_capture(Move move, Position &position) {
        const Board &board = position.board();
        Piece victim = board.get(move.dst());

        position.make(move);
        Value value =
            std::abs(PIECE_WEIGHTS[victim]) - see_target(move.dst(), position);
        switch (move.type()) {
        case MoveType::QueenPromoCapture:
        case MoveType::KnightPromoCapture:
        case MoveType::BishopPromoCapture:
        case MoveType::RookPromoCapture:
            value += board.get(move.dst());
        default:
            break;
        }
        position.undo();

        return value;
    }

    void MovePicker::add(Move move,
                         Position &position,
                         History &htable,
                         Node &node) {
        MoveEntry &entry = _entries[_length++];
        entry.move = move;

        // Determine move phase
        switch (move.type()) {
        case MoveType::Capture:
        case MoveType::QueenPromoCapture:
        case MoveType::KnightPromoCapture:
        case MoveType::BishopPromoCapture:
        case MoveType::RookPromoCapture:
        case MoveType::EnPassant:
            entry.phase = MovePhase::Captures;
            break;
        default:
            entry.phase = MovePhase::NonCapture;
            break;
        }

        // Override if it's a hash move
        bool node_type = node.type != NodeType::Invalid;
        bool node_move = node.move == move;
        bool node_hash = node.hash == position.hash();
        if (node_type && node_move && node_hash) {
            entry.phase = MovePhase::HashPhase;
        }

        // Compute move value
        switch (entry.phase) {
        case MovePhase::HashPhase:
            entry.value = MAX_MOVE_VALUE;
        case MovePhase::Captures:
            entry.value = evaluate_capture(entry.move, position);
            break;
        case MovePhase::NonCapture:
            entry.value = htable.get(position, entry.move);
            break;
        default:
            break;
        }
    }

    bool MovePicker::end() const { return _search_index == _length; }

    MoveEntry &MovePicker::next() {
        // Find the best move
        MoveIndex found_index = _search_index;
        MoveIndex phase_count = 0;
        while (_phase != MovePhase::Sentinel && phase_count == 0) {
            for (MoveIndex i = _search_index; i < _length; i++) {
                MoveEntry entry = _entries[i];
                if (entry.phase == _phase) {
                    phase_count++;
                    if (entry.value > _entries[found_index].value ||
                        _entries[found_index].phase != _phase) {
                        found_index = i;
                    }
                }
            }

            // Advance the phase if no more moves exist for it
            _phase = static_cast<MovePhase>(_phase + (phase_count == 0));
        }

        // Swap and advance the search index
        std::swap(_entries[found_index], _entries[_search_index]);
        return _entries[_search_index++];
    }

    MoveIndex MovePicker::search_index() { return _search_index; }
} // namespace Brainiac
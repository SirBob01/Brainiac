#pragma once

#include "History.hpp"
#include "MoveList.hpp"
#include "Numeric.hpp"
#include "Transpositions.hpp"

namespace Brainiac {
    /**
     * @brief Phases of the move picker.
     *
     */
    enum MovePhase : char {
        HashPhase,
        Captures,
        NonCapture,
        Sentinel,
    };

    /**
     * @brief Move entry.
     *
     */
    struct MoveEntry {
        MovePhase phase = MovePhase::Sentinel;
        Move move;
        MoveValue value;
    };

    /**
     * @brief Move registry during search. This implements move scoring and
     * sorting subroutines.
     *
     */
    class MovePicker {
        std::array<MoveEntry, MAX_MOVES_PER_TURN + 1> _entries;
        MovePhase _phase = MovePhase::HashPhase;

        MoveIndex _length = 0;
        MoveIndex _search_index = 0;

        MoveEntry _sentinel;

        MoveValue see_target(Square target, Position &position);

        /**
         * @brief Evaluate a capture.
         *
         * @param move
         * @return MoveValue
         */
        MoveValue evaluate_capture(Move move, Position &position);

      public:
        /**
         * @brief Register a move.
         *
         * @param move
         * @param position
         * @param htable
         * @param node
         */
        void add(Move move, Position &position, History &htable, Node &node);

        /**
         * @brief Check if the picker has exhausted all moves.
         *
         * @return true
         * @return false
         */
        bool end() const;

        /**
         * @brief Get the next move. This should be called after all moves have
         * been registered.
         *
         * @return MoveEntry&
         */
        MoveEntry &next();

        /**
         * @brief Get the current search index.
         *
         * @return MoveIndex
         */
        MoveIndex search_index();
    };
} // namespace Brainiac
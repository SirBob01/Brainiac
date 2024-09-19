#pragma once

#include <array>

#include "Move.hpp"
#include "Numeric.hpp"

namespace Brainiac {
    /**
     * @brief Principal variation table for search.
     *
     */
    class PVTable {
        std::array<Move, MAX_DEPTH * MAX_DEPTH> _table;
        std::array<Depth, MAX_DEPTH> _lengths;

        /**
         * @brief Get the index offset at a ply.
         *
         * @param ply
         * @return unsigned
         */
        unsigned get_offset(Depth ply) const;

      public:
        PVTable();

        /**
         * @brief Get the length of the PV at a ply.
         *
         * @param ply
         * @return unsigned
         */
        unsigned get_length(Depth ply) const;

        /**
         * @brief Get a PV move.
         *
         * @param index
         * @return Move
         */
        Move get(Depth ply, unsigned index) const;

        /**
         * @brief Update the PV at a ply.
         *
         * @param ply
         * @param last
         */
        void update(Depth ply, Move last);

        /**
         * @brief Clear the PV entries at a ply.
         *
         */
        void clear(Depth ply);
    };
} // namespace Brainiac
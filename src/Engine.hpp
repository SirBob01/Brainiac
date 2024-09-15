#include "Bitboard.hpp"
#include "Board.hpp"
#include "Evaluation.hpp"
#include "History.hpp"
#include "Move.hpp"
#include "MoveGen.hpp"
#include "MoveList.hpp"
#include "Perft.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "Search.hpp"
#include "Sliders.hpp"
#include "State.hpp"
#include "Transpositions.hpp"
#include "UCI.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief Initialize the engine and perform start-up routines here.
     *
     */
    void init();

    /**
     * @brief Get the version of the engine.
     *
     * @return std::string
     */
    std::string get_engine_version();
} // namespace Brainiac
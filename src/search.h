#ifndef BRAINIAC_SEARCH_H_
#define BRAINIAC_SEARCH_H_

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "board.h"
#include "heuristic.h"
#include "transpositions.h"

#define MAX_SCORE            100000.0f
#define SECONDS_TO_NANO      1000000000.0f
#define MAX_DEPTH            128
#define MAX_QUIESCENCE_DEPTH 32
#define FUTILITY_MARGIN      50

namespace brainiac {
    using Time = std::chrono::time_point<std::chrono::steady_clock>;

    /**
     * @brief Types of moves that will likely change the evaluation dramatically
     *
     */
    constexpr MoveFlagSet VOLATILE_MOVE_FLAGS =
        MoveFlag::Capture | MoveFlag::BishopPromo | MoveFlag::KnightPromo |
        MoveFlag::QueenPromo | MoveFlag::RookPromo | MoveFlag::EnPassant |
        MoveFlag::Castling;

    /**
     * @brief Initialize engine submodules
     *
     */
    inline void init() {
        init_rook_tables();
        init_bishop_tables();
        init_king_tables();
        init_knight_tables();
    }

    /**
     * @brief Core algorithm for strategically finding the best move
     *
     */
    class Search {
        /**
         * @brief Pairing of a move with its heuristic score for ordering
         *
         */
        struct MoveScore {
            Move move;
            float score;

            MoveScore() : move(), score(-INFINITY){};

            MoveScore(const Move &move, float score) :
                move(move), score(score){};
        };

        /**
         * @brief History heuristic value is scaled based on the number of times
         * it was visited. This means that moves are not prioritized just
         * because they are visited more.
         *
         */
        struct HistoryHeuristic {
            float _h_score = 0;
            float _b_score = 0;
        };

        double _iterative_timeout_ns;

        // Index by [piece type][to]
        HistoryHeuristic _history_heuristic[64][64] = {};

        // Index by depth
        Move _killer_moves[MAX_DEPTH + 1];

        Transpositions _transpositions;
        Time _start_time;

        // Statistic for measuring search pruning performance
        int _visited;

        /**
         * @brief Alpha-beta pruning algorithm with quiescence search, PVS, LMR,
         * and null-move pruning
         *
         * @param board Board state
         * @param alpha Lower bound
         * @param beta Upper bound
         * @param depth Current search depth
         * @param turn Current turn
         * @param move Previously executed move
         * @return float
         */
        float negamax(Board &board,
                      float alpha,
                      float beta,
                      int depth,
                      Color turn,
                      Move &move);

        /**
         * @brief Root of the negamax routine with iterative deepening
         *
         * @param board
         * @param move
         * @return float
         */
        float negamax_root(Board &board, Move &move);

        /**
         * @brief Estimate the value of a move to optimize search in
         * alpha-beta pruning
         *
         * @param board
         * @param move
         * @param depth
         * @return float
         */
        float ordering_heuristic(Board &board, const Move &move, int depth);

      public:
        Search();

        /**
         * @brief Evaluate the board position relative to white
         *
         * @param board
         * @return float
         */
        float evaluate(Board &board);

        /**
         * @brief Select the best possible move for the current position
         *
         * @param board
         * @return Move
         */
        Move move(Board &board);

        /**
         * @brief Get the current principal variation
         *
         * @param board
         * @return std::vector<Move>
         */
        std::vector<Move> get_principal_variation(Board &board);
    };
} // namespace brainiac

#endif
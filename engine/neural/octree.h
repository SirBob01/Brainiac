#ifndef CHESS_NEURAL_OCTREE_H_
#define CHESS_NEURAL_OCTREE_H_

#include <vector>

namespace chess::neural {
    struct Point3 {
        double x;
        double y;
        double z;

        bool operator==(const Point3 &other) const;
    };

    struct Point3Hash {
        std::size_t operator()(Point3 const &s) const noexcept;
    };
    
    struct Octree {
        Point3 center;
        double size;
        int level;

        double weight;

        std::vector<Octree *> children;

        Octree(Point3 center, double size, int level) 
            : center(center), 
              size(size), 
              level(level) {};
        ~Octree();

        /**
         * Generate this node's children
         */
        void generate_children();

        /**
         * Calculate the variance of the quadtree
         */
        double get_variance();
    };
}

#endif
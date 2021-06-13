#include "octree.h"

namespace chess::neural {
    bool Point3::operator==(const Point3 &other) const {
        return other.x == x && other.y == y && other.z == z;
    }

    std::size_t Point3Hash::operator()(Point3 const &s) const noexcept {
        std::size_t h1 = std::hash<int>{}(s.x);
        std::size_t h2 = std::hash<int>{}(s.y);
        std::size_t h3 = std::hash<int>{}(s.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }

    Octree::~Octree() {
        for(auto &child: children) {
            delete child;
        }
    }

    void Octree::generate_children() {
        children.push_back(new Octree({
            center.x - size/2, 
            center.y - size/2, 
            center.z - size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x + size/2, 
            center.y - size/2, 
            center.z - size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x - size/2, 
            center.y + size/2, 
            center.z - size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x - size/2, 
            center.y - size/2, 
            center.z + size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x + size/2, 
            center.y + size/2, 
            center.z - size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x + size/2, 
            center.y - size/2, 
            center.z + size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x - size/2, 
            center.y + size/2, 
            center.z + size/2}, size/2, level+1));
        children.push_back(new Octree({
            center.x + size/2, 
            center.y + size/2, 
            center.z + size/2}, size/2, level+1));
    }

    double Octree::get_variance() {
        std::vector<double> weights;
        for(auto &child : children) {
            recur_weights(child, weights);
        }
        double var =variance(weights);
        return var;
    }

    void Octree::recur_weights(Octree *root, std::vector<double> &weights) {
        if((root->children).size()) {
            for(auto child : root->children) {
                recur_weights(child, weights);
            }
        }
        else {
            weights.push_back(root->weight);
        }
    }

    double variance(std::vector<double> &values) {
        int n = values.size();
        if(n == 0) {
            return 0;
        }

        double mean = 0;
        for(auto &v : values) {
            mean += v;
        }
        mean /= n;

        double var = 0;
        for(auto &v : values) {
            double d = v - mean;
            var += d * d;
        }
        return var / n;
    }
}

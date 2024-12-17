#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>

using ld_t = long double;

struct point 
{
    std::vector<ld_t> coords;
    std::size_t index = 0;

    explicit point(std::vector<ld_t> dims, size_t idx): 
        coords(std::move(dims)), index(idx) 
    {

    }

    explicit point() = default;
};

double euclidean_distance(const point &a, const point &b) 
{
    double sum = 0;
    for (size_t i = 0; i < a.coords.size(); ++i) 
    {
        double diff = a.coords[i] - b.coords[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

class kd_tree 
{
    struct node 
    {
        point pt;
        std::unique_ptr<node> right = nullptr;
        std::unique_ptr<node> left = nullptr;
        explicit node(const point &p): 
            pt(p) 
        {

        }
    };
    
    std::unique_ptr<node> _root = nullptr;
    int _dim = 0;

    void build(std::vector<point> &points, int l, int r, std::size_t depth = 0) 
    {
        if (l > r) 
        {
            return;
        }

        std::size_t axis = depth % _dim;
        auto medianIndex = l + (r - l) / 2;

        sort(points.begin() + l, points.begin() + r + 1, 
            [&axis] (const point &a, const point &b) 
            {
                return a.coords[axis] < b.coords[axis];
            }
        );

        insert(_root, 0, points[medianIndex]);

        if (l != r) 
        {
            build(points, l, medianIndex - 1, depth + 1);
            build(points, medianIndex + 1, r, depth + 1);
        }
    }
    
    void insert(std::unique_ptr<node> &root, std::size_t depth, const point &request) 
    {
        if (root == nullptr)
        {
            root = std::make_unique<node>(request);
            return;
        }

        auto axis = depth % _dim;
        if (request.coords[axis] < root->pt.coords[axis]) 
        {
            insert(root->left, depth + 1, request);
        }
        else 
        {
            insert(root->right, depth + 1, request);
        }
    }

    void nearest_neighbor_search(const std::unique_ptr<node> &node, const point &target, std::size_t depth, double &best_dist, point &best_point) const 
    {
        if (!node) 
        {
            return;
        }

        auto dist = euclidean_distance(node->pt, target);
        auto axis = depth % _dim;
    
        if (dist < best_dist) 
        {
            best_dist = dist;
            best_point = node->pt;
        }
    
        double diff = target.coords[axis] - node->pt.coords[axis];
        const auto &first = (diff < 0) ? node->left : node->right;
        const auto &second = (diff >= 0) ? node->left : node->right;
    
        nearest_neighbor_search(first, target, depth + 1, best_dist, best_point);
    
        if (fabs(diff) < best_dist) 
        {
            nearest_neighbor_search(second, target, depth + 1, best_dist, best_point);
        }
    }

public:
    explicit kd_tree(std::vector<point> &points, std::size_t d): 
        _dim(d) 
    {
        build(points, 0, points.size() - 1, 0);
    }

    point nearest_neighbor(const point &target) const 
    {
        point best_point;
        auto best_dist = std::numeric_limits<double>::max();
        nearest_neighbor_search(_root, target, 0, best_dist, best_point);
        return best_point;
    }
};

int main() 
{
    std::ios::sync_with_stdio(false); 
    std::cin.tie(0);
    
    std::size_t n, q; 
    int d;

    std::cin >> n >> d;
    std::vector<point> points;
    points.reserve(n);

    for(size_t i = 0; i < n; ++i) 
    {
        std::vector<ld_t> dimensions;
        dimensions.resize(d);
        for(size_t j = 0; j < d; ++j) 
        {
            std::cin >> dimensions[j];
        }
        points.emplace_back(dimensions, i + 1);
    }
    kd_tree tree(points, d);
    std::cin >> q;

    for (size_t i = 0; i < q; ++i) 
    {
        std::vector<ld_t> query(d);
        for (size_t j = 0; j < d; ++j) 
        {
            std::cin >> query[j];
        }

        std::cout << tree.nearest_neighbor(point(query, 0)).index << std::endl;
    }

    return 0;
}
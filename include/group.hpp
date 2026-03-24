#pragma once

#include <vector>
#include <stddef.h>
#include <cstdint>

struct Groups final {
private:
    std::vector<int32_t> parent_or_size_; 
    // parent_or_size_[i] < 0  - i is a root of group
    // size of group is -parent_or_size_[i]

    std::vector<double> level_;
    // level_[i] is the level of group with root i

public:
    Groups() = default;
    explicit Groups(double level);

    int32_t find(int32_t x) noexcept;
    void    unite(int32_t x, int32_t y) noexcept;

    
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Groups::Groups(double level) : level_(1, level), parent_or_size_(1, -1) {}

inline int32_t Groups::find(int32_t x) noexcept {
    int32_t root = x;

    while (parent_or_size_[root] >= 0) {
        root = parent_or_size_[root];
    }

    while (x != root) {
        int32_t parent = parent_or_size_[x];
        parent_or_size_[x] = root;
        x = parent;
    }

    return root;
}

inline void Groups::unite(int32_t x, int32_t y) noexcept {
    int32_t root_x = find(x);
    int32_t root_y = find(y);

    if (root_x == root_y) return;

    int32_t size_x = -parent_or_size_[root_x];
    int32_t size_y = -parent_or_size_[root_y];

    if (size_x < size_y) {
        std::swap(root_x, root_y);
        std::swap(size_x, size_y);
    }

    const double total_level =
        (level_[root_x] * static_cast<double>(size_x) +
         level_[root_y] * static_cast<double>(size_y)) /
        static_cast<double>(size_x + size_y);

    parent_or_size_[root_x] += parent_or_size_[root_y];
    parent_or_size_[root_y] = root_x;
    level_[root_x] = total_level;
}



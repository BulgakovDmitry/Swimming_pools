#ifndef GROUP_HPP
#define GROUP_HPP

#include <vector>
#include <stddef.h>
#include <cstdint>

namespace Swimming_pool {

struct Channel {
    int32_t channel_x;
    int32_t channel_y;
    bool    is_open;

    Channel(int32_t x, int32_t y, bool is_open_flag = true) 
        : channel_x(x), channel_y(y), is_open(is_open_flag) {}
};

class Groups final {
private:
    std::vector<int32_t> parent_or_size_; 
    // parent_or_size_[i] < 0  - i is a root of group
    // size of group is -parent_or_size_[i]

    std::vector<double> level_;
    // level_[i] is the level of group with root i

public:
    Groups(int32_t N);

    void         add_group     (int32_t x, double litters_of_water) noexcept;
    int32_t      find          (int32_t x) noexcept;
    void         unite         (int32_t x, int32_t y) noexcept;
    void         add_water     (int32_t x, double litters_of_water = 0) noexcept;
    double       get_level     (int32_t x) noexcept;
    void         close_channels(const int32_t N, const std::vector<Channel>& channels);

private:
    int32_t size(int32_t root) const noexcept;
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Groups::Groups(int32_t N) : parent_or_size_(N, 0), level_(N, 0) {}

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

inline void Groups::add_water(int32_t x, double litters_of_water) noexcept {
    int32_t root_x = find(x);
    level_[root_x] += litters_of_water / size(root_x);
}

inline void Groups::add_group(int32_t x, double litters_of_water = 0) noexcept {
    parent_or_size_[x] = -1;
    level_[x] = litters_of_water;
}

inline double Groups::get_level(int32_t x) noexcept {
    int32_t root = find(x);

    return level_[root];
}

inline void Groups::close_channels(const int32_t N, const std::vector<Channel>& channels) {
    std::vector<double> water(N, 0.0);
    for (int32_t i = 0; i < N; ++i) {
        water[i] = get_level(i);
    }

    parent_or_size_.assign(N, 0);
    level_.assign(N, 0.0);

    for (int32_t i = 0; i < N; ++i) {
        add_group(i, water[i]);
    }

    for (const Channel& ch : channels) {
        if (ch.is_open) {
            unite(ch.channel_x, ch.channel_y);
        }
    }
}

inline int32_t Groups::size(int32_t root) const noexcept {
    return -parent_or_size_[root];
}

} // namespace Swimming_pool

#endif
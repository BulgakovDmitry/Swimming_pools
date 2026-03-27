#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "group.hpp"
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <fstream>

constexpr int32_t N = 38'000'000; // Количество бассейнов
constexpr int32_t K = 11'000'000; // Количество каналов
constexpr int32_t L = 29'000'000; // Количество добавлений воды
constexpr int32_t M = 3'000'000;  // Количество разрывов
constexpr int32_t A = 1, B = 500; // Количество добавляемой воды в каждый бассейн (от и до)


void create_N_groups(Groups& groups, const int32_t N);

template <typename Pool>
void connect_pulls_with_channels(Groups& groups, const int32_t K, std::vector<Channel>& channels, Pool pool);

void measure_water(Groups& groups, const int32_t N);

template <typename Water, typename Group>
void add_water_to_groups(Groups& groups, int32_t n, Water liters_of_water, Group group_index);

template <typename Generator>
void break_channels(Groups& groups, const int32_t N,
                    std::vector<Channel>& channels,
                    int32_t M,
                    Generator& gen);

void break_inds_channels(Groups& groups, const int32_t N,
                               std::vector<Channel>& channels,
                               const std::vector<int32_t>& inds);

static inline void     print_completed();
static inline void     measure(Groups& groups, const int32_t N, std::ofstream& output); // for debug
static inline void     measure(Groups& groups, const int32_t N);
static inline uint64_t make_channel_key(int32_t a, int32_t b);

void create_N_groups(Groups& groups, const int32_t N) {
    std::cout << "pools creation ... " << std::flush; 

    for (int32_t i = 0; i < N; ++i) {
        groups.add_group(i, 0);
    }

    print_completed();
}

template <typename Water, typename Group>
void add_water_to_groups(Groups& groups, int32_t n, Water liters_of_water, Group group_index) {
    std::cout << "adding water ... " << std::flush; 

    for (int32_t i = 0; i < n; ++i) {
        groups.add_water(group_index(), liters_of_water());
    }

    print_completed();
}

template <typename Pool>
void connect_pulls_with_channels(Groups& groups, const int32_t K, std::vector<Channel>& channels, Pool pool) {
    std::cout << "connect pools with channels ... " << std::flush; 
    std::unordered_set<uint64_t> used_channels;
    used_channels.reserve(static_cast<size_t>(K) * 2);
    size_t number_of_channels = 0;

    while (number_of_channels < K) {
        int32_t sp_1 = pool();
        int32_t sp_2 = pool();

        if (sp_1 == sp_2) continue;

        uint64_t key = make_channel_key(sp_1, sp_2);

        if (used_channels.count(key)) continue;

        used_channels.insert(key);
        if (sp_1 > sp_2) std::swap(sp_1, sp_2);

        channels.emplace_back(sp_1, sp_2);
        groups.unite(sp_1, sp_2);
        ++number_of_channels;
    }
    print_completed();
}

void measure_water(Groups& groups, const int32_t N) {
    std::cout << "measuring the water ... " << std::flush; 
    measure(groups, N);
    print_completed();
}

template <typename Generator>
void break_channels(Groups& groups, const int32_t N,
                    std::vector<Channel>& channels,
                    int32_t M,
                    Generator& gen) {
    std::vector<int32_t> open_channels(channels.size());
    std::iota(open_channels.begin(), open_channels.end(), 0);

    std::shuffle(open_channels.begin(), open_channels.end(), gen);

    int32_t to_close = std::min(M, static_cast<int32_t>(channels.size()));

    std::vector<int32_t> indices_to_close(open_channels.begin(),
                                         open_channels.begin() + to_close);

    break_inds_channels(groups, N, channels, indices_to_close);
}

void break_inds_channels(Groups& groups, const int32_t N,
                               std::vector<Channel>& channels,
                               const std::vector<int32_t>& inds) {
    std::cout << "breaking channels ... " << std::flush;

    for (int32_t idx : inds) {
        if (idx < channels.size()) {
            channels[idx].is_open = false;
        }
    }

    groups.close_channels(N, channels);
    print_completed();
}


static inline uint64_t make_channel_key(int32_t a, int32_t b) {
    if (a > b) {
        std::swap(a, b);
    }

    return (static_cast<uint64_t>(static_cast<uint32_t>(a)) << 32) |
           static_cast<uint32_t>(b);
}

static inline void measure(Groups& groups, const int32_t N, std::ofstream& output) {
    output << "========= measurement =========\n";
    for (int32_t i = 0; i < N; ++i) {
        output << "pool " << i << ": " << groups.get_level(i) << '\n';
    }
    output << std::endl;
}

static inline void measure(Groups& groups, const int32_t N) {
    for (int32_t i = 0; i < N; ++i) {
        groups.get_level(i);
        asm volatile("" : : "g"(i) : "memory");
    }
}

static inline void print_completed() {
    std::cout << "is completed\n";
}


class Driver final {
private:
    Groups groups;

public:
    
};

#endif // DRIVER_HPP
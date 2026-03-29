#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "group.hpp"
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <cstddef>

namespace Swimming_pool {

class Driver final {
private:
    Groups groups_;
    std::vector<Channel> channels_;

    uint32_t N_;     // Количество бассейнов
    uint32_t K_;     // Количество каналов
    uint32_t L_;     // Количество добавлений воды
    uint32_t M_;     // Количество разрывов
    uint32_t A_, B_; // Количество добавляемой воды в каждый бассейн (от и до)

public:
    Driver(const uint32_t N = 38'000'000, 
           const uint32_t K = 11'000'000,
           const uint32_t L = 29'000'000, 
           const uint32_t M = 3'000'000, 
           const uint32_t A = 1, 
           const uint32_t B = 500) 
        : groups_(N), channels_(), N_(N), K_(K), L_(L), M_(M), A_(A), B_(B) {
            channels_.reserve(K_);
        }
    
    const Groups& groups() const noexcept { return groups_; }
    Groups& groups() noexcept { return groups_; }

    const std::vector<Channel>& channels() const noexcept { return channels_; }

    uint32_t N() const noexcept {return N_;}
    uint32_t K() const noexcept {return K_;}
    uint32_t L() const noexcept {return L_;}
    uint32_t M() const noexcept {return M_;}
    uint32_t A() const noexcept {return A_;}
    uint32_t B() const noexcept {return B_;}

    void create_N_groups();

    template<typename Water_amount, typename Group_ind>
    void add_water_to_n_groups(uint32_t n, Water_amount&& liters_of_water, Group_ind&& group_index);

    template<typename Pool>
    void connect_pulls_with_channels(Pool pool);

    void measure_water();

    template<typename Generator>
    void close_channels(Generator& gen);
    
    void close_inds_channels(const std::vector<uint32_t>& inds);
    
private:
    static void print_completed();
    static uint64_t make_channel_key(uint32_t a, uint32_t b);

    void measure(std::ofstream& output);
    void measure();
    
};

inline void Driver::create_N_groups() {
    std::cout << "pools creation ... " << std::flush; 

    for (uint32_t i = 0; i < N_; ++i) {
       groups_.add_group(i, 0);
    }

    print_completed();
}

template<typename Water_amount, typename Group_ind>
void Driver::add_water_to_n_groups(uint32_t n, Water_amount&& liters_of_water, Group_ind&& group_index) {
    std::cout << "adding water ... " << std::flush; 

    for (uint32_t i = 0; i < n; ++i) {
        groups_.add_water(group_index(), liters_of_water());
    }

    print_completed();
}

inline void Driver::print_completed() {
    std::cout << "is completed\n";
}

inline uint64_t Driver::make_channel_key(uint32_t a, uint32_t b) {
    if (a > b) {
        std::swap(a, b);
    }

    return (static_cast<uint64_t>(a) << 32) | b;
}

inline void Driver::measure(std::ofstream& output) {
    output << "========= measurement =========\n";
    for (uint32_t i = 0; i < N_; ++i) {
        output << "pool " << i << ": " << groups_.get_level(i) << '\n';
    }
    output << std::endl;
}

inline void Driver::measure() {
    for (uint32_t i = 0; i < N_; ++i) {
        groups_.get_level(i);
    }
}

template<typename Pool>
void Driver::connect_pulls_with_channels(Pool pool) {
    std::cout << "connect pools with channels ... " << std::flush; 
    std::unordered_set<uint64_t> used_channels;
    used_channels.reserve(static_cast<size_t>(K_) * 2);
    uint32_t number_of_channels = 0;

    while (number_of_channels < K_) {
        uint32_t sp_1 = pool();
        uint32_t sp_2 = pool();

        if (sp_1 == sp_2) continue;

        uint64_t key = make_channel_key(sp_1, sp_2);

        if (used_channels.count(key)) continue;

        used_channels.insert(key);
        if (sp_1 > sp_2) std::swap(sp_1, sp_2);

        channels_.emplace_back(sp_1, sp_2);
        groups_.unite(sp_1, sp_2);
        ++number_of_channels;
    }
    print_completed();
}

inline void Driver::measure_water() {
    std::cout << "measuring the water ... " << std::flush; 
    measure();
    print_completed();
}

template<typename Generator>
void Driver::close_channels(Generator& gen) {
    std::vector<int32_t> open_channels(channels_.size());
    std::iota(open_channels.begin(), open_channels.end(), 0);

    std::shuffle(open_channels.begin(), open_channels.end(), gen);

    uint32_t to_close = std::min(M_, static_cast<uint32_t>(channels_.size()));

    std::vector<uint32_t> indices_to_close(open_channels.begin(),
                                         open_channels.begin() + to_close);

    close_inds_channels(indices_to_close);
}

inline void Driver::close_inds_channels(const std::vector<uint32_t>& inds) {
    std::cout << "breaking channels ... " << std::flush;

    for (uint32_t idx : inds) {
        if (idx < channels_.size()) {
            channels_[idx].is_open = false;
        }
    }

    groups_.close_channels(N_, channels_);
    print_completed();
}

} // namespace Swimming_pool 

#endif // DRIVER_HPP

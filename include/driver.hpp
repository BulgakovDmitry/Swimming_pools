#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "group.hpp"
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <cstddef>

class Driver final {
private:
    Groups groups_;
    std::vector<Channel> channels_;

    int32_t N_;     // Количество бассейнов
    int32_t K_;     // Количество каналов
    int32_t L_;     // Количество добавлений воды
    int32_t M_;     // Количество разрывов
    int32_t A_, B_; // Количество добавляемой воды в каждый бассейн (от и до)

public:
    Driver(const int32_t N = 38'000'000, 
           const int32_t K = 11'000'000,
           const int32_t L = 29'000'000, 
           const int32_t M = 3'000'000, 
           const int32_t A = 1, 
           const int32_t B = 500) 
        : groups_(N), channels_(), N_(N), K_(K), L_(L), M_(M), A_(A), B_(B) {
            channels_.reserve(K_);
        }
    
    int32_t N() const noexcept {return N_;}
    int32_t K() const noexcept {return K_;}
    int32_t L() const noexcept {return L_;}
    int32_t M() const noexcept {return M_;}
    int32_t A() const noexcept {return A_;}
    int32_t B() const noexcept {return B_;}

    void create_N_groups();

    template<typename Water, typename Group>
    void add_water_to_n_groups(int32_t n, Water liters_of_water, Group group_index);

    template<typename Pool>
    void connect_pulls_with_channels(Pool pool);

    void measure_water();

    template<typename Generator>
    void break_channels(Generator& gen);
    
    void break_inds_channels(const std::vector<int32_t>& inds);
    
private:
    static void print_completed();
    static uint64_t make_channel_key(int32_t a, int32_t b);

    void measure(std::ofstream& output);
    void measure();
    
};

void Driver::create_N_groups() {
    std::cout << "pools creation ... " << std::flush; 

    for (int32_t i = 0; i < N_; ++i) {
       groups_.add_group(i, 0);
    }

    print_completed();
}

template<typename Water, typename Group>
void Driver::add_water_to_n_groups(int32_t n, Water liters_of_water, Group group_index) {
    std::cout << "adding water ... " << std::flush; 

    for (int32_t i = 0; i < n; ++i) {
        groups_.add_water(group_index(), liters_of_water());
    }

    print_completed();
}

void Driver::print_completed() {
    std::cout << "is completed\n";
}

uint64_t Driver::make_channel_key(int32_t a, int32_t b) {
    if (a > b) {
        std::swap(a, b);
    }

    return (static_cast<uint64_t>(static_cast<uint32_t>(a)) << 32) |
           static_cast<uint32_t>(b);
}

void Driver::measure(std::ofstream& output) {
    output << "========= measurement =========\n";
    for (int32_t i = 0; i < N_; ++i) {
        output << "pool " << i << ": " << groups_.get_level(i) << '\n';
    }
    output << std::endl;
}

void Driver::measure() {
    for (int32_t i = 0; i < N_; ++i) {
        groups_.get_level(i);
        asm volatile("" : : "g"(i) : "memory");
    }
}

template<typename Pool>
void Driver::connect_pulls_with_channels(Pool pool) {
    std::cout << "connect pools with channels ... " << std::flush; 
    std::unordered_set<uint64_t> used_channels;
    used_channels.reserve(static_cast<size_t>(K_) * 2);
    size_t number_of_channels = 0;

    while (number_of_channels < K_) {
        int32_t sp_1 = pool();
        int32_t sp_2 = pool();

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

void Driver::measure_water() {
    std::cout << "measuring the water ... " << std::flush; 
    measure();
    print_completed();
}

template<typename Generator>
void Driver::break_channels(Generator& gen) {
    std::vector<int32_t> open_channels(channels_.size());
    std::iota(open_channels.begin(), open_channels.end(), 0);

    std::shuffle(open_channels.begin(), open_channels.end(), gen);

    int32_t to_close = std::min(M_, static_cast<int32_t>(channels_.size()));

    std::vector<int32_t> indices_to_close(open_channels.begin(),
                                         open_channels.begin() + to_close);

    break_inds_channels(indices_to_close);
}

void Driver::break_inds_channels(const std::vector<int32_t>& inds) {
    std::cout << "breaking channels ... " << std::flush;

    for (int32_t idx : inds) {
        if (idx < channels_.size()) {
            channels_[idx].is_open = false;
        }
    }

    groups_.close_channels(N_, channels_);
    print_completed();
}

#endif // DRIVER_HPP
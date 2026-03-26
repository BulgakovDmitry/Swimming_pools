#include <cstddef>
#include <random>
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <unordered_set>
#include "group.hpp"

static void measure(Groups& groups, const int32_t N, std::ofstream& output); // for debug
static void measure(Groups& groups, const int32_t N);
static uint64_t make_channel_key(int32_t a, int32_t b);

int main() {
    // std::string output_path = "output.txt";
    // std::ofstream output(output_path);
    // if (!output) {
    //     std::cerr << "Cannot open output file\n";
    //     return 1;
    // }

    constexpr int32_t N = 38'000'000; // Количество бассейнов
    constexpr int32_t K = 11'000'000; // Количество каналов
    constexpr int32_t L = 29'000'000; // Количество добавлений воды
    constexpr int32_t M = 3'000'000;  // Количество разрывов
    constexpr int32_t A = 1, B = 500; // Количество добавляемой воды в каждый бассейн (от и до)
    Groups groups{N};
    
    std::random_device rd;          
    std::mt19937 gen(rd());        
    std::uniform_int_distribution<> dist_add_water    (A, B);
    std::uniform_int_distribution<> dist_swimming_pool(0, N - 1);

    using clock = std::chrono::steady_clock;
    double dt = 0;
    auto t1 = clock::now();


    // 1. создать бассейны N штук ===================================
    std::cout << "step 1 ... " << std::flush; 
    for (int32_t i = 0; i < N; ++i) {
        groups.add_group(i, 0);
    }
    std::cout << "is completed\n";
    // ==============================================================


    // 2. добавить воды в каждый бассейн ============================
    std::cout << "step 2 ... " << std::flush; 
    for (int32_t i = 0; i < N; ++i) {
        groups.add_water(i, dist_add_water(gen));
    }
    std::cout << "is completed\n";
    // ==============================================================


    // 3. K бассейнов случайно соединить каналами ===================
    std::cout << "step 3 ... " <<std::flush; 
    std::unordered_set<uint64_t> used_channels;
    used_channels.reserve(static_cast<size_t>(K) * 2);
    std::vector<Channel> channels;
    channels.reserve(K);
    size_t number_of_channels = 0;

    while (number_of_channels < K) {
        int32_t sp_1 = dist_swimming_pool(gen);
        int32_t sp_2 = dist_swimming_pool(gen);

        if (sp_1 == sp_2) continue;

        uint64_t key = make_channel_key(sp_1, sp_2);

        if (used_channels.count(key)) continue;

        used_channels.insert(key);
        if (sp_1 > sp_2) std::swap(sp_1, sp_2);

        channels.emplace_back(sp_1, sp_2);
        groups.unite(sp_1, sp_2);
        ++number_of_channels;
    }
    std::cout << "is completed\n";
    // ==============================================================


    // 4. измерить воду в каждом бассейне ===========================
    std::cout << "step 4 ... " << std::flush; 
    measure(groups, N);
    std::cout << "is completed\n";
    // ==============================================================


    // 5. вновь добавить воды в L бассейнов =========================
    std::cout << "step 5 ... " << std::flush; 
    for (size_t i = 0; i < L; ++i) {
        int32_t sp = dist_swimming_pool(gen);
        groups.add_water(sp, dist_add_water(gen));
    }
    std::cout << "is completed\n";
    // ==============================================================


    // 6. вновь измерить ============================================
    std::cout << "step 6 ... " << std::flush; 
    measure(groups, N);
    std::cout << "is completed\n";
    // ==============================================================


    // 7. Разорвать M каналов между бассейнами ======================
    std::cout << "step 7 ... " << std::flush; 
    std::vector<size_t> open_channels(channels.size());
    std::iota(open_channels.begin(), open_channels.end(), 0);

    std::shuffle(open_channels.begin(), open_channels.end(), gen);

    size_t to_close = std::min(static_cast<size_t>(M), channels.size());

    for (size_t i = 0; i < to_close; ++i) {
        channels[open_channels[i]].is_open = false;
    }

    groups.close_channels(N, channels);
    std::cout << "is completed\n";
    // ==============================================================


    // 8. вновь добавить воды в L бассейнов =========================
    std::cout << "step 8 ... " << std::flush; 
    for (size_t i = 0; i < L; ++i) {
        int32_t sp = dist_swimming_pool(gen);
        groups.add_water(sp, dist_add_water(gen));
    }
    std::cout << "is completed\n";
    // ==============================================================


    // 9. вновь измерить ============================================
    std::cout << "step 9 ... " << std::flush; 
    measure(groups, N);
    std::cout << "is completed\n";
    // ============================================================== 


    auto t2 = clock::now();
    dt = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "total time: " << dt << " ms\n";
}

static void measure(Groups& groups, const int32_t N, std::ofstream& output) {
    output << "========= measurement =========\n";
    for (int32_t i = 0; i < N; ++i) {
        output << "pool " << i << ": " << groups.get_level(i) << '\n';
    }
    output << std::endl;
}

static void measure(Groups& groups, const int32_t N) {
    for (int32_t i = 0; i < N; ++i) {
        groups.get_level(i);
        asm volatile("" : : "g"(i) : "memory");
    }
}

static uint64_t make_channel_key(int32_t a, int32_t b) {
    if (a > b) {
        std::swap(a, b);
    }

    return (static_cast<uint64_t>(static_cast<uint32_t>(a)) << 32) |
           static_cast<uint32_t>(b);
}
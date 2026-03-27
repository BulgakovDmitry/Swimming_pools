#include <cstddef>
#include <random>
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <unordered_set>
#include "group.hpp"
#include "driver.hpp"

int main() {
    
    Groups groups{N};
    
    std::random_device rd;          
    std::mt19937 gen(rd());        
    std::uniform_int_distribution<> dist_add_water    (A, B);
    std::uniform_int_distribution<> dist_swimming_pool(0, N - 1);

    using clock = std::chrono::steady_clock;
    double dt = 0;
    auto t1 = clock::now();

    // 1. создать бассейны N штук ===================================
    create_N_groups(groups, N);

    // 2. добавить воды в каждый бассейн ============================
    int32_t group_index = 0;
    add_water_to_groups(groups, N, [&](){return dist_add_water(gen);}, [&](){return group_index++;});

    // 3. K бассейнов случайно соединить каналами ===================
    std::vector<Channel> channels;
    channels.reserve(K);
    connect_pulls_with_channels(groups, K, channels, [&](){return dist_swimming_pool(gen);});

    // 4. измерить воду в каждом бассейне ===========================
    measure_water(groups, N);

    // 5. вновь добавить воды в L бассейнов =========================
    add_water_to_groups(groups, L, [&](){return dist_add_water(gen);}, [&](){return dist_swimming_pool(gen);});

    // 6. вновь измерить ============================================
    measure_water(groups, N);

    // 7. Разорвать M каналов между бассейнами ======================
    break_channels(groups, N, channels, M, gen);

    // 8. вновь добавить воды в L бассейнов =========================
    add_water_to_groups(groups, L, [&](){return dist_add_water(gen);}, [&](){return dist_swimming_pool(gen);});

    // 9. вновь измерить ============================================
    measure_water(groups, N);

    auto t2 = clock::now();
    dt = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "total time: " << dt << " ms\n";
}


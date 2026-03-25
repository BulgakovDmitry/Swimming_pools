#include <cstddef>
#include <set>
#include <random>
#include <iostream>
#include <fstream>
#include <chrono>
#include "group.hpp"

static void measure(std::ofstream& output, Groups& groups, const int32_t N);

int main() {
    std::string output_path = "output.txt";
    std::ofstream output(output_path);
    if (!output) {
        std::cerr << "Cannot open output file\n";
        return 1;
    }

    double dt = 0;
    using clock = std::chrono::steady_clock;

    const int32_t N = 5;          // Количество бассейнов
    const int32_t K = 2;          // Количество каналов
    const int32_t L = 2;          // Количество добавлений воды
    const int32_t M = 3;          // Количество разрывов
    const int32_t A = 1, B = 500; // Количество добавляемой воды в каждый бассейн (от и до)
    Groups groups{N};
    

    std::random_device rd;          
    std::mt19937 gen(rd());        
    std::uniform_int_distribution<> dist_add_water    (A, B);
    std::uniform_int_distribution<> dist_swimming_pool(0, N - 1);

    auto t1 = clock::now();
    // 1. создать бассейны N штук ===================================
    for (int32_t i = 0; i < N; ++i) {
        groups.add_group(i, static_cast<double>(i + 2));
    }
    // ==============================================================

    // 2. добавить воды в каждый бассейн ============================
    for (int32_t i = 0; i < N; ++i) {
        groups.add_water(i, dist_add_water(gen));
    }
    // ==============================================================
    measure(output, groups, N);

    // 3. K бассейнов случайно соединить каналами ===================
    std::set<std::pair<int32_t, int32_t>> used_channels;
    size_t number_of_channels = 0;

    while (number_of_channels < K) {
        int32_t sp_1 = dist_swimming_pool(gen);
        int32_t sp_2 = dist_swimming_pool(gen);

        if (sp_1 == sp_2) continue;

        if (sp_1 > sp_2) std::swap(sp_1, sp_2);

        if (used_channels.count({sp_1, sp_2})) continue;

        used_channels.insert({sp_1, sp_2});
        groups.unite(sp_1, sp_2);
        ++number_of_channels;
    }
    // ==============================================================

    // 4. измерить воду в каждом бассейне ===========================
    measure(output, groups, N);
    // ==============================================================

    // 5. вновь добавить воды в L бассейнов =========================
    for (size_t i = 0; i < L; ++i) {
        int32_t sp = dist_swimming_pool(gen);
        groups.add_water(sp, dist_add_water(gen));
    }
    // ==============================================================

    // 6. вновь измерить ============================================
    measure(output, groups, N);
    // ==============================================================

    // 7. Разорвать M каналов между бассейнами ======================
    
    // ==============================================================

    // 8. вновь добавить воды в L бассейнов =========================
    for (size_t i = 0; i < L; ++i) {
        int32_t sp = dist_swimming_pool(gen);
        groups.add_water(sp, dist_add_water(gen));
    }
    // ==============================================================

    // 9. вновь измерить ============================================
    measure(output, groups, N);
    // ==============================================================    
    auto t2 = clock::now();

    dt = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "total time: " << dt << " ms\n";
}

static void measure(std::ofstream& output, Groups& groups, const int32_t N) {
    output << "========= measurement =========\n";
    for (int32_t i = 0; i < N; ++i) {
        output << "channel " << i << ": " << groups.get_level(i) << '\n';
    }
    output << std::endl;
}

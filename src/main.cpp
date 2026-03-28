#include <random>
#include <chrono>
#include "group.hpp"
#include "driver.hpp"

int main() {
    Swimming_pool::Driver driver{};

    std::random_device rd;          
    std::mt19937 gen(rd());        
    std::uniform_int_distribution<> dist_add_water    (driver.A(), driver.B());
    std::uniform_int_distribution<> dist_swimming_pool(0, driver.N() - 1);
    
    using clock = std::chrono::steady_clock;
    double dt = 0;
    auto t1 = clock::now();

    // 1. создать бассейны N штук ===================================
    driver.create_N_groups();

    // 2. добавить воды в каждый бассейн ============================
    int32_t group_index = 0;
    driver.add_water_to_n_groups(driver.N(), 
        [&](){return dist_add_water(gen);}, 
        [&](){return group_index++;});

    // 3. K бассейнов случайно соединить каналами ===================
    driver.connect_pulls_with_channels([&](){return dist_swimming_pool(gen);});

    // 4. измерить воду в каждом бассейне ===========================
    driver.measure_water();

    // 5. вновь добавить воды в L бассейнов =========================
    driver.add_water_to_n_groups(driver.L(), 
        [&](){return dist_add_water(gen);}, 
        [&](){return dist_swimming_pool(gen);});

    // 6. вновь измерить ============================================
    driver.measure_water();

    // 7. Разорвать M каналов между бассейнами ======================
    driver.close_channels(gen);
    
    // 8. вновь добавить воды в L бассейнов =========================
    driver.add_water_to_n_groups(driver.L(), 
        [&](){return dist_add_water(gen);}, 
        [&](){return dist_swimming_pool(gen);});

    // 9. вновь измерить ============================================
    driver.measure_water();

    auto t2 = clock::now();
    dt = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "total time: " << dt << " ms\n";
}


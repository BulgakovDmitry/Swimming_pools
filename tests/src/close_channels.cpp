#include <gtest/gtest.h>
#include <vector>
#include <random>
#include "driver.hpp"

// ==================================================================
// This is a test of the method:
// template<typename Generator>
// void close_channels(Generator& gen);
// ==================================================================

TEST(close_channels, does_nothing_when_M_is_zero) {
    Swimming_pool::Driver driver(3, 1, 0, 0, 1, 10);
    driver.create_N_groups();

    std::vector<int> pools = {0, 1};
    std::size_t pos = 0;
    auto pool = [&]() { return pools[pos++]; };
    driver.connect_pulls_with_channels(pool);

    driver.add_water_to_n_groups(
        1,
        []() { return 10.0; },
        []() { return 0; }
    );

    const double before0 = driver.groups().get_level(0);
    const double before1 = driver.groups().get_level(1);
    const double before2 = driver.groups().get_level(2);
    const std::size_t before_channels = driver.channels().size();

    std::mt19937 gen(42);
    driver.close_channels(gen);

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), before0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), before1);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(2), before2);
    EXPECT_EQ(driver.channels().size(), before_channels);
}

TEST(close_channels, closing_single_channel_disconnects_pools) {
    Swimming_pool::Driver driver(2, 1, 0, 1, 1, 10);
    driver.create_N_groups();

    std::vector<int> pools = {0, 1};
    std::size_t pos = 0;
    auto pool = [&]() { return pools[pos++]; };
    driver.connect_pulls_with_channels(pool);

    driver.add_water_to_n_groups(
        1,
        []() { return 10.0; },
        []() { return 0; }
    );

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 5.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 5.0);

    std::mt19937 gen(123);
    driver.close_channels(gen);

    driver.add_water_to_n_groups(
        1,
        []() { return 10.0; },
        []() { return 0; }
    );

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 15.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 5.0);
}
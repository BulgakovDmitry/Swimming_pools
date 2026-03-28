#include <gtest/gtest.h>
#include "driver.hpp"

// ==================================================================
// This is a test of the method:
// template<typename Water, typename Group>
// void add_water_to_n_groups(int32_t n, Water liters_of_water, Group group_index);
// ==================================================================

TEST(add_water_to_n_groups, adds_water_to_single_pool) {
    Swimming_pool::Driver driver(3, 2, 1, 1, 1, 10);
    driver.create_N_groups();

    auto water = []() { return 7.0; };
    auto group = []() { return 1; };

    driver.add_water_to_n_groups(1, water, group);

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 0.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 7.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(2), 0.0);
}

TEST(add_water_to_n_groups, adds_water_to_several_pools) {
    Swimming_pool::Driver driver(3, 3, 2, 1, 1, 10);
    driver.create_N_groups();

    std::vector<int> groups = {0, 1, 2};

    std::size_t pos = 0;
    auto water = []() { return 5.0; };
    auto group = [&]() { return groups[pos++]; };

    driver.add_water_to_n_groups(3, water, group);

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 5.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 5.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(2), 5.0);
}

TEST(add_water_to_n_groups, accumulates_water_in_same_pool) {
    Swimming_pool::Driver driver(2, 0, 0, 0, 1, 10);
    driver.create_N_groups();

    auto water = []() { return 4.0; };
    auto group = []() { return 0; };

    driver.add_water_to_n_groups(3, water, group);

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 12.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 0.0);
}

TEST(add_water_to_n_groups, uses_new_water_value_each_iteration) {
    Swimming_pool::Driver driver(2, 1, 1, 1, 1, 10);
    driver.create_N_groups();

    const std::vector<double> waters = {1.0, 2.0, 3.0};
    std::size_t water_pos = 0;

    auto water = [&]() { return waters[water_pos++]; };
    auto group = []() { return 1; };

    driver.add_water_to_n_groups(3, water, group);

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 0.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 6.0);
}

TEST(add_water_to_n_groups, does_nothing_when_n_is_zero) {
    Swimming_pool::Driver driver(3, 0, 0, 0, 1, 10);
    driver.create_N_groups();

    auto water = []() { return 100.0; };
    auto group = []() { return 2; };

    driver.add_water_to_n_groups(0, water, group);

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 0.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), 0.0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(2), 0.0);
}
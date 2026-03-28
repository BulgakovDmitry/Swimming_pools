#include <gtest/gtest.h>
#include "driver.hpp"

// ==================================================================
// This is a test of the method:
// void create_N_groups();
// ==================================================================

TEST(create_N_groups, one_group) {
    Swimming_pool::Driver driver(1, 0, 0, 0, 1, 10);
    driver.create_N_groups();

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), 0.0);
}

TEST(create_N_groups, five_groups) {
    Swimming_pool::Driver driver(5, 0, 0, 0, 1, 10);
    driver.create_N_groups();

    for (int32_t i = 0; i < driver.N(); ++i) {
        EXPECT_DOUBLE_EQ(driver.groups().get_level(i), 0.0);
    }
}

TEST(create_N_groups, hundred_groups) {
    Swimming_pool::Driver driver(100, 20, 15, 10, 5, 10);
    driver.create_N_groups();

    for (int32_t i = 0; i < driver.N(); ++i) {
        EXPECT_DOUBLE_EQ(driver.groups().get_level(i), 0.0);
    }
}


#include <gtest/gtest.h>
#include "driver.hpp"

// ==================================================================
// This is a test of the method:
// void measure_water();
// ==================================================================

TEST(measure_water, does_not_change_state) {
    Swimming_pool::Driver driver(3, 1, 0, 0, 1, 10);
    driver.create_N_groups();

    std::vector<int> pools = {0, 1};
    std::size_t count = 0;
    auto pool = [&]() { return pools[count++]; };
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

    EXPECT_DOUBLE_EQ(driver.groups().get_level(0), before0);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(1), before1);
    EXPECT_DOUBLE_EQ(driver.groups().get_level(2), before2);
    EXPECT_EQ(driver.channels().size(), before_channels);
}

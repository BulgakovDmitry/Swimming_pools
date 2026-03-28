#include <gtest/gtest.h>
#include "driver.hpp"

// ==================================================================
// This is a test of the method:
// template<typename Pool>
// void connect_pulls_with_channels(Pool pool);
// ==================================================================

TEST(connect_pulls_with_channels, creates_no_channels_when_K_is_zero) {
    Swimming_pool::Driver driver(4, 0, 0, 0, 1, 10);
    driver.create_N_groups();

    auto pool = []() { return 0; };

    driver.connect_pulls_with_channels(pool);

    EXPECT_TRUE(driver.channels().empty());
}

TEST(connect_pulls_with_channels, creates_exactly_one_channel) {
    Swimming_pool::Driver driver(3, 1, 0, 0, 1, 10);
    driver.create_N_groups();

    std::vector<int> values = {0, 1};
    std::size_t pos = 0;

    auto pool = [&]() { return values[pos++]; };

    driver.connect_pulls_with_channels(pool);

    ASSERT_EQ(driver.channels().size(), 1u);
    EXPECT_NE(driver.channels()[0].channel_x, driver.channels()[0].channel_y);
}

TEST(connect_pulls_with_channels, skips_self_loops) {
    Swimming_pool::Driver driver(3, 1, 0, 0, 1, 10);
    driver.create_N_groups();

    std::vector<int> values = {
        1, 1,  
        0, 2   
    };
    std::size_t pos = 0;

    auto pool = [&]() { return values[pos++]; };

    driver.connect_pulls_with_channels(pool);

    ASSERT_EQ(driver.channels().size(), 1u);
    EXPECT_NE(driver.channels()[0].channel_x, driver.channels()[0].channel_y);
    EXPECT_TRUE(
        (driver.channels()[0].channel_x == 0 && driver.channels()[0].channel_y == 2) ||
        (driver.channels()[0].channel_x == 2 && driver.channels()[0].channel_y == 0)
    );
}

TEST(connect_pulls_with_channels, skips_duplicate_channels) {
    Swimming_pool::Driver driver(4, 2, 0, 0, 1, 10);
    driver.create_N_groups();

    std::vector<int> values = {
        0, 1,   
        1, 0,    
        2, 3     
    };
    std::size_t pos = 0;

    auto pool = [&]() { return values[pos++]; };

    driver.connect_pulls_with_channels(pool);

    ASSERT_EQ(driver.channels().size(), 2u);

    const auto& c0 = driver.channels()[0];
    const auto& c1 = driver.channels()[1];

    EXPECT_NE(c0.channel_x, c0.channel_y);
    EXPECT_NE(c1.channel_x, c1.channel_y);
}

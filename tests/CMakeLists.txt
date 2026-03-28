find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include(GoogleTest)

set(SRC_LIST
    src/create_groups.cpp
    src/add_water.cpp
    src/connect_pools.cpp
    src/measure_water.cpp
    src/close_channels.cpp
)

add_executable(driver ${SRC_LIST})

target_link_libraries(driver
    PRIVATE 
        swimming_pools::headers
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

gtest_discover_tests(driver
    PROPERTIES LABELS "unit"
)          
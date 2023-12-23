add_executable(
        VMTest
        VMTest.cpp
)
target_link_libraries(
        VMTest
        vm
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(VMTest)
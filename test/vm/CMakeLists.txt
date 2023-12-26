add_executable(
        VMTest
        VMTest.cpp
)
target_link_libraries(
        VMTest
        vm
        GTest::gtest_main
)

add_executable(
        VMWithParserTest
        VMWithParserTest.cpp
)
target_link_libraries(
        VMWithParserTest
        vm
        GTest::gtest_main
)

add_executable(
        GCTest
        GCTest.cpp
)
target_link_libraries(
        GCTest
        vm
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(VMTest)
gtest_discover_tests(VMWithParserTest)
gtest_discover_tests(GCTest)

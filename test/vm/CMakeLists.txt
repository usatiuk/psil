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

add_executable(
        CompilerTest
        CompilerTest.cpp
)
target_link_libraries(
        CompilerTest
        vm
        GTest::gtest_main
)


include(GoogleTest)
gtest_discover_tests(VMTest DISCOVERY_TIMEOUT 600)
gtest_discover_tests(VMWithParserTest DISCOVERY_TIMEOUT 600)
gtest_discover_tests(GCTest DISCOVERY_TIMEOUT 600)
gtest_discover_tests(CompilerTest DISCOVERY_TIMEOUT 600)

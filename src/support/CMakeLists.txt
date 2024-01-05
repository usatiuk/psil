set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(support src/Logger.cpp src/Options.cpp)

target_link_libraries(support PRIVATE Threads::Threads)

target_include_directories(support PUBLIC include)

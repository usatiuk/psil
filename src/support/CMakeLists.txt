set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(support src/Logger.cpp src/Options.cpp)

target_include_directories(support PUBLIC include)

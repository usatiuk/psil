set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(logger src/Logger.cpp)

target_include_directories(logger PUBLIC include)

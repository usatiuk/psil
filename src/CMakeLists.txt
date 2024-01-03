set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vm)
add_subdirectory(logger)

add_executable(psil main.cpp)

target_link_libraries(psil PRIVATE vm)

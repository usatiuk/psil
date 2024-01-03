cmake_minimum_required(VERSION 3.20)
project(psil)

if (SANITIZE STREQUAL "YES")
    message(WARNING "Enabling sanitizers!")
    add_compile_options(-Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal -D_GLIBCXX_DEBUG -Wconversion -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2)
    add_compile_options(-fsanitize=address -fsanitize=undefined -fno-sanitize-recover)
    add_link_options(-fsanitize=address -fsanitize=undefined -fno-sanitize-recover)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-flto)
    add_link_options(-flto)
endif ()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O3)
    add_link_options(-O3)
endif ()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

enable_testing()

add_subdirectory(test)
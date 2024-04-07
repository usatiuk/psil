set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(vm src/VM.cpp src/Cell.cpp
        src/Parser.cpp src/MemoryContext.cpp src/Handle.cpp src/Compiler.cpp)

if (NOT NO_THREADS)
    find_package(Threads REQUIRED)
    target_link_libraries(vm PRIVATE Threads::Threads)
else ()
    target_compile_options(vm PUBLIC -DNO_THREADS)
endif ()

target_link_libraries(vm PUBLIC support)

target_include_directories(vm PUBLIC include)

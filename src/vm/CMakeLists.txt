set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(vm src/VM.cpp src/Cell.cpp
        src/Parser.cpp src/MemoryContext.cpp src/Handle.cpp src/Compiler.cpp)

target_link_libraries(vm PUBLIC logger)

target_include_directories(vm PUBLIC include)

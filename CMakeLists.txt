cmake_minimum_required(VERSION 3.16)
project(skiplist
    VERSION 1.0
    LANGUAGES CXX
)

# ---- Library ----

add_library(skiplist
    src/memorytools.cpp
)

target_include_directories(skiplist
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(skiplist
    PUBLIC cxx_std_17
)

# ---- Example ----

add_executable(skiplist_basic
    examples/basic.cpp
)

target_link_libraries(skiplist_basic
    PRIVATE skiplist
)
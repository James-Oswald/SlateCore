
cmake_minimum_required(VERSION 3.16)
project("SlateCore" VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fstack-protector -s")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(SlateCore SHARED)
target_sources(SlateCore PRIVATE
    src/Fomula_io.cpp
    src/Formula_methods.cpp
    src/Formula_predicate.cpp
    src/SExpression.cpp
    src/Term.cpp
    src/verify.cpp
) 

enable_testing()
add_subdirectory(tests)
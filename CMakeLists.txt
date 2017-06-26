cmake_minimum_required(VERSION 2.6)
project(CAP)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(cap ${SOURCES})
target_compile_features(cap PRIVATE cxx_range_for)

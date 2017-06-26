cmake_minimum_required(VERSION 2.6)
project(CAP)

set (CAP_VERSION_MAJOR 1)
set (CAP_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/CAPConfig.h.in"
  "${PROJECT_BINARY_DIR}/CAPConfig.h"
)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(cap main.cpp)
target_compile_features(cap PRIVATE cxx_range_for)

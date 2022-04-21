cmake_minimum_required(VERSION 3.14)
project(oram_posmap)

set(CMAKE_CXX_STANDARD 14)

add_executable(oram_posmap main.cpp include/position_map.h alg/array.h alg/hist_mem.h compressed/roaring.hh compressed/roaring.c compressed/roaring.h include/functions.h alg/counter_interval.h compressed/dynamic/dynamic.hpp compressed/dynamic/internal/wt_string.hpp include/node.h include/leaf_contents.h)
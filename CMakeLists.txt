cmake_minimum_required(VERSION 3.14)
project(oram_posmap)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -ggdb3")

add_executable(oram_posmap main.cpp include/position_map.h alg/array.h alg/hist_mem.h compressed/roaring.hh compressed/roaring.c compressed/roaring.h include/wavelet_tree.h alg/counter_interval.h compressed/dynamic/dynamic.hpp compressed/dynamic/internal/wt_string.hpp include/node.h include/leaf_contents.h oram/h_oram.h alg/benchmark.h include/wavelet_tree.h include/run_length.h)
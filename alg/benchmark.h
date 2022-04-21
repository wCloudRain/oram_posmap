//
// Created by wholland on 21/04/22.
//

#ifndef ORAM_POSMAP_BENCHMARK_H
#define ORAM_POSMAP_BENCHMARK_H

#include "../include/position_map.h"

class benchmark : public position_map
{
protected:

public:

    explicit benchmark(uint32_t size) : position_map(size) {

    }

    void add_address(address addr) override {
       // do nothing
    }

    void add_level_offset(address add, uint32_t level, uint32_t offset) override {
        // do nothing
    }

};

#endif //ORAM_POSMAP_BENCHMARK_H

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
        // do nothing
    }

    void add_address(address addr) override {
       // do nothing
    }

    void add_level(address add, uint32_t level) override {
        // do nothing
    }

    uint32_t auxiliary_info(address addr) override {
        // do nothing
        return 0;
    }

    uint32_t level_query(address addr) override {
        // do nothing
        return 0;
    }


};

#endif //ORAM_POSMAP_BENCHMARK_H

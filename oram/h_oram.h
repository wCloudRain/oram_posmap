//
// Created by wholland on 21/04/22.
//

#ifndef ORAM_POSMAP_H_ORAM_H
#define ORAM_POSMAP_H_ORAM_H

#include "server.h"
#include "../include/position_map.h"
#include "../alg/benchmark.h"
#include "../alg/array.h"
#include "../alg/hist_mem.h"
#include "../alg/counter_interval.h"

#define BENCHMARK 0
#define ARRAY 1
#define HISTORICAL 2
#define COUNTER 3

class h_oram {

private:

    uint32_t size;
    server *cloud;
    position_map *pm;

public:

    h_oram(uint32_t size, uint32_t type, uint32_t block_size) :
        size(size),
        cloud(new server(block_size))
    {
        switch (type) {
            case BENCHMARK:
                pm = new benchmark(size);
                break;
            case ARRAY:
                pm = new array_map(size);
                break;
            case HISTORICAL:
                pm = new hist_mem(size);
            case COUNTER:
                pm = new counter_interval(size, 100);
        }
    }

    void
};

#endif //ORAM_POSMAP_H_ORAM_H

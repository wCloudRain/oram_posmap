//
// Created by wholland on 21/04/22.
//

#ifndef ORAM_POSMAP_H_ORAM_H
#define ORAM_POSMAP_H_ORAM_H

#include "server.h"
#include "../include/position_map.h"
#include "../include/functions.h"
#include "../alg/benchmark.h"
#include "../alg/array.h"
#include "../alg/hist_mem.h"
#include "../alg/counter_interval.h"

#define BENCHMARK 0
#define ARRAY 1
#define HISTORICAL 2
#define COUNTER 3
#define AUX 0

// focus is on throughout and memory allocation for the position map files
class h_oram {

private:

    uint32_t count;
    uint32_t size;
    server *cloud;
    position_map *pm;
    uint32_t L;
    uint32_t *level_sizes;

public:

    h_oram(uint32_t size, uint32_t type, uint32_t block_size) :
        size(size),
        count(0),
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

        L = ceil(log2(size));;
        level_sizes = (uint32_t*) calloc(L, sizeof(uint32_t));

        // level_sizes[0] = 1;
        for (int i = 0; i < L+1; ++i) {
            level_sizes[i] = ((uint32_t) pow(2, i)) ;
        }

        for (int i = 0; i < L+1; ++i) {
            cloud->create_array(i, level_sizes[i]);
        }

        int index;
        for (int i = 0; i < L; ++i) {
            for (index = 0; index < level_sizes[i]; ++index) {
                cloud->put(i, index, new element(UINT32_MAX, AUX, nullptr));
            }
        }

        for (index = 0; index < size; ++index) {
            cloud->put(L, index, new element(index, 0, nullptr));
        }
        for (int i = index; i < level_sizes[L]; ++i) {
            cloud->put(L, i, new element(UINT32_MAX, 0, nullptr));
        }
    }

    void access(address addr) {

        // simulation. we do not perform actual access

        // update position map
        // rebuild
        count++;
        uint32_t index = 0;
        int rebuild_level = lsb(count);
        printf("Put %d in level %d\n", addr, rebuild_level);
        cloud->put(rebuild_level, index, new element(addr, AUX, nullptr));
        pm->add_level_offset(addr, rebuild_level, index);
        index++;

        if(rebuild_level != L) {
            merge_levels(rebuild_level, rebuild_level, index);
        } else {
            uint32_t temp_array = L+1;
            cloud->create_array(temp_array, level_sizes[L]);
            merge_levels(rebuild_level+1, temp_array, index);
            cloud->swap_and_delete(temp_array, L);
        }
        pm->add_address(addr);
    }

    void merge_levels(int rebuild_level, int output_array, uint32_t index) {

        element *e;
        for (int i = 0; i < rebuild_level; ++i) {
            for (int j = 0; j < level_sizes[i]; ++j) {
                e = cloud->get(i, j);
                if(e->key != INT32_MAX) {
                    cloud->put(output_array, index, e);
                    index++;
                    pm->add_level_offset(e->key, rebuild_level, index);
                }
            }
            // erase array
            for (int j = 0; j < level_sizes[i]; ++j) {
                cloud->put(i, j, new element(UINT32_MAX, AUX, nullptr));
            }
        }
        for (int j = index; j < level_sizes[rebuild_level]; ++j) {
            cloud->put(output_array, j, new element(UINT32_MAX, AUX, nullptr));
        }
    }

    void print() {
        element *e;
        for (int i = 0; i < L+1; ++i) {
            printf("LEVEL %d :: |", i);
            for (int j = 0; j < level_sizes[i]; ++j) {
                e = cloud->get(i, j);
                if(e->key != INT32_MAX) {
                    printf(" %d |", e->key);
                }
                delete e;
            }
            printf("\n");
        }
    }

    uint32_t level_query(address addr) {
        return pm->level_query(addr);
    }
};

#endif //ORAM_POSMAP_H_ORAM_H

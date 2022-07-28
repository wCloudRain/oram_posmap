//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_ARRAY_H
#define ORAM_POSMAP_ARRAY_H

#include <cstdlib>
#include "../include/position_map.h"

class array_map : public position_map
{
protected:
    uint8_t *levels;
    uint32_t *offsets;
    uint16_t *counts;

public:

    explicit array_map(uint64_t size) :
            position_map(size),
            levels((uint8_t *) calloc(size, sizeof(uint8_t))),
            counts((uint16_t *) calloc(size, sizeof(uint16_t))),
            offsets((uint32_t *) calloc(size, sizeof(uint32_t)))
        {
        printf("array length: %lu\n", size);
        }

    ~array_map() {
        delete levels;
        delete counts;
    }

    void add_address(address addr) override {
        // print();
        levels[addr];
        counts[addr]++;
    }

    void add_level(address addr, uint32_t level) override {
        levels[addr] = (uint8_t) level;
    }

    uint32_t auxiliary_info(address addr) override {
        return counts[addr];
    }

    uint32_t level_query(address addr) override {
        return levels[addr];
    }

    void print() {
        printf("ARRAY: [");
        for (int j = 0; j < 10; ++j) {
            printf("(%d, %d), ", j, counts[j]);
        }
        printf("]\n");
    }

};

#endif //ORAM_POSMAP_ARRAY_H

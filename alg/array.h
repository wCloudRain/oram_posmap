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
    uint32_t *levels;
    uint32_t *offsets;
    uint16_t *counts;

public:

    explicit array_map(uint32_t size) :
            position_map(size),
            levels((uint32_t*) calloc(size, sizeof(uint32_t))),
            offsets((uint32_t*) calloc(size, sizeof(uint32_t))),
            counts((uint16_t *) calloc(size, sizeof(uint16_t)))
        {}

    void add_address(address addr) override {
        offsets[addr];
        levels[addr];
        counts[addr]++;
    }

    void add_level_offset(address add, uint32_t level, uint32_t offset) override {
        offsets[add] = offset;
        levels[add] = level;
    }

};

#endif //ORAM_POSMAP_ARRAY_H

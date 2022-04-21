//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_ARRAY_H
#define ORAM_POSMAP_ARRAY_H

#include <cstdlib>
#include "../include/position_map.h"

class array : public position_map
{
protected:
    uint32_t *levels;
    uint32_t *offsets;

public:

    explicit array(uint32_t size) : position_map(size) {
        this->levels = (uint32_t*) calloc(size, sizeof(uint32_t));
        this->offsets = (uint32_t*) calloc(size, sizeof(uint32_t));
    }

    void add_address(address add) override {
        levels[add] = 0;
    }

    void change_offset(address add, uint32_t offset) {
        offsets[add] = offset;
    }

    void change_level(address add, uint32_t level) {
        levels[add] = level;
    }
};

#endif //ORAM_POSMAP_ARRAY_H

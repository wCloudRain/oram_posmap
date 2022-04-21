//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_POSITION_MAP_H
#define ORAM_POSMAP_POSITION_MAP_H

#include <stdint-gcc.h>

typedef uint32_t address;

class position_map
{
protected:
    uint32_t size;

public:

    explicit position_map(uint32_t size):
    size(size)
    {}

    virtual void add_address(address add) {}

    virtual void add_level_offset(address add, uint32_t level, uint32_t offset) {}

};

#endif //ORAM_POSMAP_POSITION_MAP_H


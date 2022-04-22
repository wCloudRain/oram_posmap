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
    uint32_t L;

public:

    explicit position_map(uint32_t size):
    size(size),
    L(ceil(log2(size)))
    {}

    virtual void add_address(address add) {}

    virtual void add_level_offset(address add, uint32_t level, uint32_t offset) {}

    virtual uint32_t auxiliary_info(address add) {}

    virtual uint32_t level_query(address add) {}
};

#endif //ORAM_POSMAP_POSITION_MAP_H


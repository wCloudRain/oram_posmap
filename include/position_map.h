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

};

#endif //ORAM_POSMAP_POSITION_MAP_H


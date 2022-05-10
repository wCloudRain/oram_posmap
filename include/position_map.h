//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_POSITION_MAP_H
#define ORAM_POSMAP_POSITION_MAP_H

#include <stdint-gcc.h>

typedef uint64_t address;

class position_map
{
protected:
    address size;
    uint32_t L;

public:

    explicit position_map(address size):
    size(size),
    L(ceil(log2(size)))
    {}

    virtual void add_address(address add) {}

    virtual void add_level(address add, uint32_t level) {}

    virtual uint32_t auxiliary_info(address add) {}

    virtual uint32_t level_query(address add) {}
};


static int lsb(uint32_t value)
{
    int i16 = !(value & 0xffff) << 4;
    value >>= i16;

    int i8 = !(value & 0xff) << 3;
    value >>= i8;

    int i4 = !(value & 0xf) << 2;
    value >>= i4;

    int i2 = !(value & 0x3) << 1;
    value >>= i2;

    int i1 = !(value & 0x1);

    int i0 = (value >> i1) & 1? 0 : -32;

    return i16 + i8 + i4 + i2 + i1 + i0;
}

#endif //ORAM_POSMAP_POSITION_MAP_H


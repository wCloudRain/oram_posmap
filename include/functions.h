//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_FUNCTIONS_H
#define ORAM_POSMAP_FUNCTIONS_H

#include <cstdint>

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

#endif //ORAM_POSMAP_FUNCTIONS_H

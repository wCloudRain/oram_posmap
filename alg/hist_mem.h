//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_HIST_MEM_H
#define ORAM_POSMAP_HIST_MEM_H

#include <cstdlib>
#include <vector>
#include "../include/position_map.h"
#include "../compressed/roaring.hh"
#include "../include/functions.h"

typedef Roaring dictionary;
typedef std::vector<bool> bitvector;

#define EMPTY false
#define FULL true

class hist_mem : public position_map
{
protected:
    dictionary **levels;
    dictionary *cache;
    bitvector *occupied;

    uint32_t L;
    uint32_t count;
    uint32_t num_cache_levels;

public:

    explicit hist_mem(uint32_t size) :
        position_map(size),
        count(0) {

        L = 32 - __builtin_clz(size);
        num_cache_levels = L/2;
        L = L-  num_cache_levels;

        printf("L: %d\n", L);
        occupied =  new bitvector(L, false);
        levels = (dictionary**) calloc(L, sizeof(dictionary*));
        cache = new dictionary;
    }

    void add_address(address addr) override {

        // simulate accessing position information
        level_query(addr);
        rank_query(addr);

        cache->add(addr);
        count++;

        int rebuild_level = lsb(count) - num_cache_levels;

        if(rebuild_level >= 0) {
            dictionary *merged_level = cache;

            for (int i = 0; i < rebuild_level; ++i) {
                if(rebuild_level != L) {
                    *merged_level |= *levels[i];
                }
                delete levels[i];
                occupied->at(i) = EMPTY;
            }

            if(rebuild_level != L) {
                levels[rebuild_level] = merged_level;
                occupied->at(rebuild_level) = FULL;
            } else {
                delete merged_level;
            }
            cache = new dictionary();
        }

        if(count == size) {
            count = 0;
        }
    }

    uint32_t level_query(address addr) {
        for (int i = 0; i < L-1; ++i) {
            if(occupied->at(i)) {
                if(levels[i]->contains(addr)) {
                    return i;
                }
            }
        }
        return L;
    }

    uint32_t rank_query(address addr) {
        uint32_t level = level_query(addr);

        assert(level <= L);

        if(level == L) {
            return addr;
        } else {
            return levels[level]->rank(addr);
        }
    }

    void add_level_offset(address add, uint32_t level, uint32_t offset) override {}
};

#endif //ORAM_POSMAP_HIST_MEM_H

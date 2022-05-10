//
// Created by wholland on 18/04/22.
//

#ifndef ORAM_POSMAP_HIST_MEM_H
#define ORAM_POSMAP_HIST_MEM_H

#include <cstdlib>
#include <vector>
#include <tgmath.h>
#include "../include/position_map.h"
#include "../compressed/roaring.hh"

typedef Roaring dictionary;
typedef std::vector<bool> bitvector;

#define EMPTY false
#define FULL true
#define CACHE 0xFFFFFFFF


class hist_mem : public position_map
{
protected:
    dictionary **levels;
    dictionary *cache;
    bitvector *occupied;

    uint64_t count;
    uint64_t cycle;
    uint32_t num_cache_levels;

public:

    explicit hist_mem(uint64_t size) :
        position_map(size),
        count(0) {

        cycle = (uint64_t) pow(2,L);
        num_cache_levels = L/2;
        L = L-  num_cache_levels;

        printf("L: %d\n", L);
        occupied =  new bitvector(L, false);
        levels = (dictionary**) calloc(L, sizeof(dictionary*));
        cache = new dictionary();
    }

    ~hist_mem() {
        for (int i = 0; i < L; ++i) {
            delete levels[i];
        }
        delete cache;
        delete occupied;
    };

    void add_address(address addr) override {

        // simulate accessing position information
        // uint32_t level = level_query(addr);
        uint32_t rank = rank_query(addr);

        cache->add(addr);
        count++;
        const int rebuild_level = lsb(count) - num_cache_levels;
        if(rebuild_level >= 0) {

            if(rebuild_level == 0) {
                levels[rebuild_level] = cache;
                occupied->at(rebuild_level) = FULL;
            } else if(rebuild_level != L) {
                std::vector<const dictionary*> inputs;
                inputs.emplace_back(cache);
                for (int l = 0; l < rebuild_level; ++l) {
                    inputs.emplace_back(levels[l]);
                }
                levels[rebuild_level] = new dictionary();
                *levels[rebuild_level] = Roaring::fastunion(rebuild_level+1, inputs.data());
                occupied->at(rebuild_level) = FULL;
            }

            if(rebuild_level != 0) {
                delete cache;
            }
            cache = new dictionary();

            for (int i = 0; i < rebuild_level; ++i) {
                delete levels[i];
                occupied->at(i) = EMPTY;
            }
        }
        if(count == cycle) {
            count = 0;
        }
    }

    uint32_t level_query(address addr) override {
        if(cache->contains(addr)) {
            return CACHE;
        }
        for (int i = 0; i < L; ++i) {
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

        if(level == CACHE) {
            return cache->rank(addr);
        } else if(level == L) {
            return addr;
        } else {
            return levels[level]->rank(addr);
        }
    }

    uint32_t auxiliary_info(address addr) override {
        return level_query(addr);
    }

    void print() {
        printf("new update >> \n");
        printf("cache: ");
        cache->printf();
        printf("\n");
        for (int j = 0; j < L; ++j) {
            printf("level %d: ",j);
            if(occupied->at(j)) {
                levels[j]->printf();
            }
            printf("\n");
        }
    }

    void add_level(address add, uint32_t level) override {}
};

#endif //ORAM_POSMAP_HIST_MEM_H

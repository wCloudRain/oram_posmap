//
// Created by wholland on 21/04/22.
//

#ifndef ORAM_POSMAP_H_ORAM_H
#define ORAM_POSMAP_H_ORAM_H

#include <iostream>
#include "../include/position_map.h"
#include "../alg/benchmark.h"
#include "../alg/array.h"
#include "../alg/hist_mem.h"
#include "../alg/compressed_metadata.h"

#define BENCHMARK 0
#define ARRAY 1
#define HISTORICAL 2
#define COUNTER 3
#define LEVEL_ZERO 0



using namespace std;

// focus is on throughout and memory allocation for the position map files
class h_oram {

private:

    uint64_t count;
    uint64_t cycle;
    uint64_t size;
    //position_map *pm;
    uint32_t L;
    set<uint64_t> **levels;
    //uint32_t *level_sizes;
    ofstream ofs;



public:

    h_oram(address size, uint32_t type, string file_name) :
        size(size),
        count(0)
    {
        /*
        switch (type) {
            case BENCHMARK:
                pm = new benchmark(size);
                break;
            case ARRAY:
                printf("POS MAP = ARRAY\n");
                pm = new array_map(size);
                break;
            case HISTORICAL:
                printf("POS MAP = HISTORICAL\n");
                pm = new hist_mem(size);
                break;
            case COUNTER:
                printf("POS MAP = COUNTER INTERVAL\n");
                pm = new compressed_metadata(size, 50);
                break;
        }
         */
        L = ceil(log2(size));;
        cycle = pow(2,L);
        printf("NUMBER OF LEVELS: %d\n", L);
        int index;
        levels =  (set<uint64_t>**) calloc(L, sizeof(set<uint64_t>**));
        for (int l = 0; l < L; ++l) {
            levels[l] = new set<uint64_t>();
        }
        string file_path = "/home/student.unimelb.edu.au/wholland/Dropbox/oram_posmap/data/test/";
        string file_name_output = file_path + file_name + "-level.txt";
        ofs.open(file_name_output);
        printf("initialization complete\n");
    }

    ~h_oram() {
        for (int j = 0; j < L; ++j) {
            delete levels[j];
        }
        // delete pm;
        ofs.close();
    }

    void access(const address addr) {
        ofs << "a," <<  addr << "\n";
        // simulation. we do not perform actual access

        // update position map
        // rebuild

        count++;
        levels[LEVEL_ZERO]->insert(addr);
        // pm->add_level(addr, LEVEL_ZERO);
        ofs << "l," <<  addr << "," << LEVEL_ZERO << "\n";

        int rebuild_level = lsb(count);

        set<uint64_t> *merged_level;
        if(rebuild_level == L) {
            merged_level = new set<uint64_t>();
        } else {
            merged_level = levels[rebuild_level];
        }

        // merge levels
        for (int l = 0; l < rebuild_level; ++l) {
            set<uint64_t> *level = levels[l];
            if(rebuild_level != L) {
                for (unsigned long it : *level) {
                    merged_level->insert(it);
                }
            }
            levels[l]->clear();
        }

        // update position map
        if(rebuild_level != LEVEL_ZERO) {
            for (unsigned long it : *merged_level) {
                // pm->add_level(it, rebuild_level);
                ofs << "l," <<  it << "," << rebuild_level << "\n";
            }
        }
        // pm->add_address(addr);

        if(rebuild_level == L) {
            delete merged_level;
        }

        if(count==cycle) {
            count = 0;
        }
    }

    void print() {

        for (int i = 0; i < L; ++i) {
            printf("LEVEL %d :: |", i);
            set<uint64_t> *level = levels[i];
            for (unsigned long it : *level) {
                printf(" %lu |", it);
            }
            printf("\n");
        }
        printf("LEVEL %d :: |", L);
        for (int j = 0; j < size; ++j) {
            printf(" %d |", j);
        }
        printf("\n");
    }

    /*
    uint32_t level_query(address addr) {
        return pm->level_query(addr);
    }

    uint32_t aux_query(address addr) {
        return pm->auxiliary_info(addr);
    }
     */
};

#endif //ORAM_POSMAP_H_ORAM_H

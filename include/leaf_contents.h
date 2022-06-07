//
// Created by wholland on 20/04/22.
//

#ifndef ORAM_POSMAP_LEAF_CONTENTS_H
#define ORAM_POSMAP_LEAF_CONTENTS_H

#include <cstdint>
#include <assert.h>
#include <cstdlib>
#include <cstdio>
#include "position_map.h"

#define LEFT true
#define RIGHT false

class leaf_contents {

protected:
    uint16_t cardinality;

public:
    uint64_t *index;
    uint16_t *values;

    explicit leaf_contents(uint32_t width, uint32_t cardinality, uint16_t value) :
            cardinality(cardinality)
    {
        index = (uint64_t*) calloc(2*width+1, sizeof(uint64_t*));
        values = (uint16_t*) calloc(2*width+1, sizeof(uint16_t*));
        values[0] = value;
    }

    explicit leaf_contents(uint32_t width, uint32_t cardinality) :
            cardinality(cardinality)
    {
        index = (uint64_t*) calloc(2*width+1, sizeof(uint64_t*));
        values = (uint16_t*) calloc(2*width+1, sizeof(uint16_t*));
    }

    ~leaf_contents() {
        delete index;
        delete values;
    }

    uint32_t count() {
        return cardinality;
    }

    uint32_t addr_count(address addr) {
        uint32_t offset = find_offset(addr);

        return values[offset];
    }

    uint32_t find_offset(address addr) {
        uint32_t offset;
        if(addr >= index[cardinality-1]) {
            offset = cardinality-1;
        } else {
            offset = 0;
            while((addr >= index[offset+1])) {
                offset++;
            }
        }
        return offset;
    }

    void update_value(address addr, uint16_t new_value) {

        assert(addr >= index[0]);

        // find the correct offset
        uint32_t offset = find_offset(addr);

        // is addr a boundary case?
        bool lboundary_case = (addr == index[offset]), rboundary_case = false;
        if(offset != (cardinality-1)) {
            // check if there is a right boundary
            rboundary_case = (addr == (index[offset+1]-1));
        }
        uint32_t old_value = values[offset], left_value, right_value;

        // for the boundary cases
        bool absorb_left = false, absorb_right = false;
        if(offset > 0) {
            absorb_left = (values[offset-1] == new_value);
        }
        if(offset < (cardinality-1)) {
            absorb_right = (values[offset+1] == new_value);
        }

        if(lboundary_case && rboundary_case) {

            singleton(offset, absorb_left, absorb_right, new_value);

        } else if(lboundary_case) {

            left_boundary(offset, absorb_left, new_value);

        } else if(rboundary_case) {

            right_boundary(offset, absorb_right, new_value);

        } else {

            insert_middle(offset, addr, new_value);

        }
    }

    void singleton(uint32_t offset, bool absorb_left, bool absorb_right, uint16_t new_value) {
        // address is a singleton
        if (absorb_left && absorb_right) {
            // delete the interval and right interval
            shift(LEFT, offset, 2);
        } else if (absorb_left || absorb_right) {
            // singleton absorbed
            shift(LEFT, offset, 1);
            if (absorb_right) {
                // shift the interval to the right
                index[offset]--;
            }
        } else {
            // singleton not absorbed
            values[offset] = new_value;
        }
    }

    void left_boundary(uint32_t offset, bool absorb, uint16_t new_value) {
        // left boundary case
        if(absorb) {
            index[offset]++;
        } else {
            shift(RIGHT, offset, 1);
            // insert new interval
            index[offset+1]++;
            values[offset] = new_value;
        }
    }

    void right_boundary(uint32_t offset, bool absorb, uint16_t new_value) {
        //printf("CASE: right boundary\n");
        // right boundary case
        if(absorb) {
            index[offset+1]--;
        } else {
            shift(RIGHT, offset+1, 1);
            // insert new interval
            index[offset+1] = index[offset+2]-1;
            values[offset+1] = new_value;
        }
    }

    void insert_middle(uint32_t offset, address addr, uint16_t new_value) {

        //printf("CASE: middle\n");
        shift(RIGHT, offset+1, 2);
        index[offset+1] = addr;
        values[offset+1] = new_value;
        index[offset+2] = addr+1;
        values[offset+2] = values[offset];
    }

    void shift(bool left, uint32_t st_offset, uint32_t shift_size) {
        // left shift decrease cardinality
        // right shift increase cardinality
        if(left) {
            //printf(" <--- left shift at offset %d and shift size %d\n", st_offset, shift_size);
            for (int i = st_offset; i < cardinality; ++i) {
                index[i] = index[i+shift_size];
                values[i] = values[i+shift_size];
            }
            cardinality -= shift_size;
        } else {
            //printf(" ---> right shift at offset %d and shift size %d\n", st_offset, shift_size);
            for (int i = cardinality-1; i>((int) st_offset)-1; i--) {
                //printf("%d goes to %d\n", i, i+shift_size);
                index[i+shift_size] = index[i];
                values[i+shift_size] = values[i];
            }
            cardinality += shift_size;
        }

    }

    leaf_contents *split(uint32_t width) {
        auto *right_contents = new leaf_contents(width, width);

        // a split node has 2*width array items
        cardinality -= width;
        for (int i = 0; i < width; ++i) {
            right_contents->index[i] = index[i+cardinality];
            right_contents->values[i] = values[i+cardinality];
        }
        return right_contents;
    }

    void merge(leaf_contents *left_contents, leaf_contents *right_contents) {
        // move the right contents in the left contents
        uint32_t left_card = left_contents->cardinality;
        for (int i = 0; i < left_card; ++i) {
            index[i] = left_contents->index[i];
            values[i] = right_contents->values[i];
        }
        for (int i = 0; i < right_contents->cardinality; ++i) {
            index[left_card+i] = right_contents->index[i];
            values[left_card+i] = right_contents->values[i];
        }
        cardinality = left_card + right_contents->cardinality;
    }

    void print() {

        printf("CONTENTS : ");
        for (int i = 0; i < cardinality; ++i) {
               printf(" (%lu, %d)|", index[i], values[i]);
        }
        printf("\n---cardinality: %d---\n", cardinality);
    }
};

#endif //ORAM_POSMAP_LEAF_CONTENTS_H

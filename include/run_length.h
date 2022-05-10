//
// Created by wholland on 11/05/22.
//

#ifndef ORAM_POSMAP_RUN_LENGTH_H
#define ORAM_POSMAP_RUN_LENGTH_H

class run_length {
protected:
    uint16_t cardinality;

public:
    uint64_t *index;
    uint16_t *counts;

    explicit leaf_contents(uint32_t width, uint32_t cardinality) :
    cardinality(cardinality)
            {
                    index = (uint64_t *) calloc(2*width+1, sizeof(uint64_t *));
            counts = (uint16_t*) calloc(2*width+1, sizeof(uint16_t*));
            }

    ~leaf_contents() {
        delete index;
        delete counts;
    }

    uint32_t count() {
        return cardinality;
    }

    uint32_t addr_count(address addr) {
        uint32_t offset = find_offset(addr);

        return counts[offset];
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

    void update(address addr) {

        // check address belongs to correct interval
        assert(addr >= index[0]);

        // find the correct offset
        uint32_t offset = find_offset(addr);

        // is addr a boundary case?
        bool lboundary_case = (addr == index[offset]), rboundary_case = false;
        if(offset != (cardinality-1)) {
            // check if there is a right boundary
            rboundary_case = (addr == (index[offset+1]-1));
        }
        uint32_t count = counts[offset], left_count, right_count;

        // for the boundary cases
        bool absorb_left = false, absorb_right = false;
        if(offset > 0) {
            absorb_left = (counts[offset-1] == (count +1));
        }
        if(offset < (cardinality-1)) {
            absorb_right = (counts[offset+1] == (count +1));
        }

        if(lboundary_case && rboundary_case) {

            singleton(offset, absorb_left, absorb_right);

        } else if(lboundary_case) {

            left_boundary(offset, absorb_left);

        } else if(rboundary_case) {

            right_boundary(offset, absorb_right);

        } else {

            insert_middle(offset, addr);

        }
    }

    void singleton(uint32_t offset, bool absorb_left, bool absorb_right) {
        //printf("CASE: singleton\n");
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
            counts[offset]++;
        }
    }

    void left_boundary(uint32_t offset, bool absorb) {
        //printf("CASE: left boundary\n");
        // left boundary case
        if(absorb) {
            index[offset]++;
        } else {
            shift(RIGHT, offset, 1);
            // insert new interval
            index[offset+1]++;
            counts[offset] = counts[offset+1]+1;
        }
    }

    void right_boundary(uint32_t offset, bool absorb) {
        //printf("CASE: right boundary\n");
        // right boundary case
        if(absorb) {
            index[offset+1]--;
        } else {
            shift(RIGHT, offset+1, 1);
            // insert new interval
            index[offset+1] = index[offset+2]-1;
            counts[offset+1] = counts[offset]+1;
        }
    }

    void insert_middle(uint32_t offset, address addr) {

        //printf("CASE: middle\n");
        shift(RIGHT, offset+1, 2);
        index[offset+1] = addr;
        counts[offset+1] = counts[offset]+1;
        index[offset+2] = addr+1;
        counts[offset+2] = counts[offset];
    }

    void shift(bool left, uint32_t st_offset, uint32_t shift_size) {
        // left shift decrease cardinality
        // right shift increase cardinality
        if(left) {
            //printf(" <--- left shift at offset %d and shift size %d\n", st_offset, shift_size);
            for (int i = st_offset; i < cardinality; ++i) {
                index[i] = index[i+shift_size];
                counts[i] = counts[i+shift_size];
            }
            cardinality -= shift_size;
        } else {
            //printf(" ---> right shift at offset %d and shift size %d\n", st_offset, shift_size);
            for (int i = cardinality-1; i>((int) st_offset)-1; i--) {
                //printf("%d goes to %d\n", i, i+shift_size);
                index[i+shift_size] = index[i];
                counts[i+shift_size] = counts[i];
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
            right_contents->counts[i] = counts[i+cardinality];
        }
        return right_contents;
    }

    void merge(leaf_contents *right_contents) {
        // move the right contents in the left contents
        for (int i = 0; i < right_contents->cardinality; ++i) {
            index[cardinality+i] = right_contents->index[i];
            counts[cardinality+i] = right_contents->counts[i];
        }
        cardinality += right_contents->cardinality;
    }

    void print() {

        printf("CONTENTS : ");
        for (int i = 0; i < cardinality; ++i) {
            printf(" (%lu, %d)|", index[i], counts[i]);
        }
        printf("\n---cardinality: %d---\n", cardinality);
    }
};

#endif //ORAM_POSMAP_RUN_LENGTH_H

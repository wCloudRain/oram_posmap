#include <iostream>
#include "compressed/roaring.hh"
#include "alg/hist_mem.h"
#include "include/leaf_contents.h"
#include "alg/counter_interval.h"
#include "compressed/dynamic/dynamic.hpp"

typedef dyn::wt_str compressed_string;

int main() {

    auto *cstr = new compressed_string(40);

    cstr->insert()













    /*
    auto *hm = new hist_mem(100);


    for (int i = 0; i < 1500; ++i) {
        address addr = i % 100;
        hm->add_address(addr, 0);
    }

    printf("here\n");
    uint32_t output;
    for (int i = 0; i < 100; ++i) {
        output = hm->rank_query(i);
        printf("----\naddress %d: rank = %d\n", i, output);
        output = hm->level_query(i);
        printf("address %d: level = %d\n", i, output);
    }

    std::vector<bool> *vec;

    vec = new std::vector<bool>(150, false);
    */


    /*
    leaf_contents *leaf = new leaf_contents(10, 1);

    counter_interval *ci = new counter_interval(200, 5);

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 10; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 10; i < 20; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 20; i < 22; ++i) {
        for (int j = 0; j < 150; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 40; i < 45; ++i) {
        for (int j = 0; j < 10; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 50; i < 60; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 60; i < 75; ++i) {
        for (int j = 0; j < 150; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 80; i < 82; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    for (int i = 85; i < 88; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    for (int i = 90; i < 92; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();

    ci->add_address(55);
    ci->add_address(81);


    ci->add_address(41);


    ci->print_tree();
    ci->print_leaf_contents();

    ci->add_address(47);

    ci->print_tree();
    ci->print_leaf_contents();


    for (int i = 180; i < 182; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    for (int i = 185; i < 188; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    for (int i = 190; i < 192; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();


    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            ci->add_address(i);
        }
    }


    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 5; i < 10; ++i) {
        for (int j = 0; j < 15; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();


    ci->add_address(40);

    ci->print_tree();
    ci->print_leaf_contents();

    for (int i = 42; i < 45; ++i) {
        for (int j = 0; j < 1; ++j) {
            ci->add_address(i);
        }
    }

    ci->print_tree();
    ci->print_leaf_contents();
    */
}
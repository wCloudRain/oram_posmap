//
// Created by wholland on 19/04/22.
//

#ifndef ORAM_POSMAP_NODE_H
#define ORAM_POSMAP_NODE_H

#include <cstdint>
#include <cstdlib>
#include <assert.h>
#include "position_map.h"
#include "leaf_contents.h"

class node {

public:
    node *parent;
    node *lchild;
    node *rchild;
    leaf_contents *contents;
    uint32_t left_index;
    uint16_t height;

    explicit node(uint32_t left_index) :
            parent(nullptr),
            lchild(nullptr),
            rchild(nullptr),
            contents(nullptr),
            left_index(left_index),
            height(1)
    {}

    explicit node(node *parent, leaf_contents *contents) :
            parent(parent),
            lchild(nullptr),
            rchild(nullptr),
            contents(contents),
            height(1)
    {
        left_index = contents->index[0];
    }

    ~node() {
        delete contents;
    }

    bool check_split(uint32_t width) {
        return contents->count() >= 2.0*width;
    }

    bool check_merge(uint32_t width) {
        // check sibling count
        if(lchild->contents == nullptr) {
            return false;
        } else if (rchild->contents == nullptr) {
            return false;
        } else {
            uint32_t left = lchild->contents->count();
            uint32_t right = rchild->contents->count();
            return (left+right) < 2.0*width;
        }
    }

    void split(uint32_t width) {
        leaf_contents *right_contents = contents->split(width);

        lchild = new node(this, contents);
        rchild = new node(this, right_contents);

        //height++;
        contents = nullptr;
    }

    void merge(uint16_t width) {
        leaf_contents *left = lchild->contents, *right = rchild->contents;

        contents = new leaf_contents(width, left->count() + right->count());
        contents->merge(left, right);
        // height = 1;
    }

    void delete_siblings() {
        delete lchild;
        delete rchild;
        lchild = nullptr;
        rchild = nullptr;
    }

    uint32_t calc_height() {
        int left, right;
        if(lchild != nullptr) {
            left = lchild->calc_height()+1;
        } else {
            left = 0;
        }
        if(rchild != nullptr) {
            right = rchild->calc_height()+1;
        } else {
            right = 0;
        }
        return (left > right) ? left : right;
    }

    int calculate_balance() {
            int left_height = (lchild == nullptr) ? 0 : lchild->height;
            int right_height = (rchild == nullptr) ? 0 : rchild->height;
            return right_height - left_height;
    }
};

#endif //ORAM_POSMAP_NODE_H

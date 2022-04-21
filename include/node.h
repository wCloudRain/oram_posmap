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

class node
{
public:
    node *parent;
    node *lchild;
    node *rchild;
    leaf_contents *contents;
    uint32_t left_index;
    int8_t balance;
    bool is_rchild;

    explicit node(uint32_t left_index) :
            parent(nullptr),
            lchild(nullptr),
            rchild(nullptr),
            contents(nullptr),
            left_index(left_index),
            balance(0),
            is_rchild(false)
    {}

    explicit node(node *parent, leaf_contents *contents) :
            parent(parent),
            lchild(nullptr),
            rchild(nullptr),
            contents(contents),
            balance(0),
            is_rchild(false)
    {
        left_index = contents->index[0];
    }


    bool check_split(uint32_t width) {
        return contents->count() >= 2*width;
    }

    bool check_merge(uint32_t width) {
        // check sibling count
        if(parent == nullptr) {
            return false;
        } else {
            uint32_t count;
            if(is_rchild) {
                count = parent->lchild->contents->count();
            } else {
                count = parent->rchild->contents->count();
            }
            count += contents->count();
            return count < 1.5 * width;
        }
    }

    void split(uint32_t width) {
        leaf_contents *right_contents = contents->split(width);

        lchild = new node(this, contents);
        rchild = new node(this, right_contents);
        rchild->is_rchild = true;

        contents = nullptr;
    }

    void merge() {
        leaf_contents *left = parent->lchild->contents, *right = parent->rchild->contents;

        left->merge(right);
        parent->contents = left;
    }

    void delete_siblings() {
        delete lchild;
        delete rchild;
        lchild = nullptr;
        rchild = nullptr;
    }
};

#endif //ORAM_POSMAP_NODE_H

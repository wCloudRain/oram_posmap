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
    //int8_t balance;
    uint16_t depth;
    bool is_rchild;

    explicit node(uint32_t left_index) :
            parent(nullptr),
            lchild(nullptr),
            rchild(nullptr),
            contents(nullptr),
            left_index(left_index),
            //balance(0),
            is_rchild(false),
            depth(0)
    {}

    explicit node(node *parent, leaf_contents *contents) :
            parent(parent),
            lchild(nullptr),
            rchild(nullptr),
            contents(contents),
            //balance(0),
            is_rchild(false),
            depth(0)
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
        if(parent == nullptr) {
            return false;
        } else {
            uint32_t count;
            if(is_rchild) {
                if(parent->lchild->contents == nullptr) {
                    return false;
                } else {
                    count = parent->lchild->contents->count();
                }
            } else {
                if(parent->rchild->contents == nullptr) {
                    return false;
                } else {
                    count = parent->rchild->contents->count();
                }
            }
            count += contents->count();
            return count < 1.0 * width;
        }
    }

    void split(uint32_t width) {
        leaf_contents *right_contents = contents->split(width);

        lchild = new node(this, contents);
        rchild = new node(this, right_contents);
        rchild->is_rchild = true;

        depth++;
        contents = nullptr;
    }

    void merge(uint16_t width) {
        leaf_contents *left = lchild->contents, *right = rchild->contents;

        contents = new leaf_contents(width, 0);
        contents->merge(left, right);
        depth = 0;
    }

    void delete_siblings() {
        delete lchild;
        delete rchild;
        lchild = nullptr;
        rchild = nullptr;
    }

    uint32_t calc_depth() {
        int left, right;
        if(lchild != nullptr) {
            left = lchild->calc_depth()+1;
        } else {
            left = 0;
        }
        if(rchild != nullptr) {
            right = rchild->calc_depth()+1;
        } else {
            right = 0;
        }
        return (left > right) ? left : right;
    }

    int calculate_balance() {
            int left_depth = (lchild == nullptr) ? 0 : lchild->depth;
            int right_depth = (rchild == nullptr) ? 0 : rchild->depth;
            return right_depth - left_depth;
    }
};

#endif //ORAM_POSMAP_NODE_H

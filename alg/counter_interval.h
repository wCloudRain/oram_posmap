//
// Created by wholland on 19/04/22.
//

#ifndef ORAM_POSMAP_COUNTER_INTERVAL_H
#define ORAM_POSMAP_COUNTER_INTERVAL_H

#include <cstdint>
#include <cstdlib>
#include <tr1/unordered_map>
#include <assert.h>
#include <iostream>
#include "../include/position_map.h"
#include "../include/node.h"
#include "../compressed/dynamic/dynamic.hpp"

#define LEFT true
#define RIGHT false
#define INCREASE true
#define DECREASE false

typedef dyn::wt_str compressed_string;

class counter_interval : public position_map
{
protected:
    node *root;
    uint32_t width;
    compressed_string *levels;

public:

    explicit counter_interval(uint32_t size, uint32_t width) :
        position_map(size),
        width(width)
    {
        root = new node(0);
        root->contents = new leaf_contents(width, 1);

        vector<pair<dyn::wt_str::char_type, double>> probabilities;
        double prob = 0.5;
        for (int i = L-1; i > 0; i--) {
            probabilities.emplace_back(i, prob);
            prob /= ((double) 2);
        }
        probabilities.emplace_back(0, prob);

        levels = new compressed_string(probabilities);
        for (int j = 0; j < size; ++j) {
            levels->insert(j, L-1);
        }
    }

    void add_address(address addr) override {

        // retrieve count and level (this is simulating what we would do)
        level_query(addr);
        count_query(addr);

        // find leaf that contains address
        node *leaf = find_leaf(addr, root);
        assert(leaf->contents != nullptr);

        leaf->contents->update(addr);

        if(leaf->check_split(width)) {

            leaf->split(width);
            height_change(leaf, INCREASE);
        } else if(leaf->check_merge(width)) {
            leaf->merge();
            height_change(leaf->parent, DECREASE);
            leaf->parent->delete_siblings();
        }
    }

    void add_level_offset(address addr, uint32_t level, uint32_t offset) override {
        levels->remove(addr);
        levels->insert(addr, level);
    }

    uint32_t auxiliary_info(address addr) override {
        return count_query(addr);
    }

    uint32_t level_query(address addr) override {
        return levels->at(addr);
    }

    uint32_t count_query(address addr) {
        node *leaf = find_leaf(addr, root);
        return leaf->contents->addr_count(addr);
    }

    node *find_leaf(address addr, node *curr_node) {
        if((curr_node->lchild == nullptr) && (curr_node->rchild == nullptr)) {
            // at leaf node
           return curr_node;
        } else {
            if(addr < curr_node->rchild->left_index) {
                // branch into left node
                find_leaf(addr, curr_node->lchild);
            } else {
                // branch into right node
                find_leaf(addr, curr_node->rchild);
            }
        }
    }

    void rotate(node *parent, node *child, bool left) {
        node *X = (left) ? child->lchild : child->rchild;
        if(X != nullptr) {
            X->parent = parent;
            X->is_rchild = left;
        }
        node* grandparent = parent->parent;
        // check if a change in root occurred.
        if(root == parent) {
            root = child;
        }
        else {
            if(parent->is_rchild) {
                grandparent->rchild = child;
                child->is_rchild = true;
            } else {
                grandparent->lchild = child;
                child->is_rchild = false;
            }
        }
        // rotate the parent node
        if(left) {
            parent->rchild = X;
            child->lchild = parent;
            // update left index
            child->left_index = parent->left_index;
        } else {
            parent->lchild = X;
            child->rchild = parent;
            // update left index
            parent->left_index = X->left_index;
        }
        parent->is_rchild = !left;
        child->parent = grandparent;
        parent->parent = child;
        // restore balance
        child->balance = 0;
        parent->balance = 0;
    }

    void height_change(node *prev, bool increase) {
        uint32_t increment_right = (increase) ? 1 : -1;
        uint32_t increment_left = (increase) ? -1 : 1;

        printf("height change for node %d\n", prev->left_index);
        bool flag = false;
        node *curr = prev->parent;
        // the parent node of the height increase is always balanced
        prev->balance = 0;
        while(curr != nullptr && !flag) {
            curr->balance += prev->is_rchild ? increment_right : increment_left;
            // check for violation of the balance criteria
            if(curr->balance > 1 || curr->balance < -1) {
                // restructure brings whole tree into balance
                flag = true;
                node *tall_child, *X;
                switch(curr->balance) {
                    case 2:
                        tall_child = curr->rchild;
                        switch(tall_child->balance) {
                            case 1:
                                rotate(curr, tall_child, LEFT);
                                break;
                            case -1:
                                X = tall_child->lchild;
                                rotate(tall_child, X, RIGHT);
                                rotate(curr, X, LEFT);
                                break;
                        }
                        break;
                    case -2:
                        tall_child = curr->lchild;
                        switch(tall_child->balance) {
                            case 1:
                                X = tall_child->rchild;
                                rotate(tall_child, X, LEFT);
                                rotate(curr, X, RIGHT);
                                break;
                            case -1:
                                rotate(curr, tall_child, RIGHT);
                                break;
                        }
                        break;
                }
            }
            prev = curr;
            curr = curr->parent;
        }
    }

    // printing functions

    static void print_tree(const std::string& prefix, const node *node, bool isLeft)
    {
        if( node != nullptr ) {
            std::cout << prefix;

            std::cout << (isLeft ? "├──" : "└──" );

            // print the value of the node
            std::string print = std::to_string( node->is_rchild);
            print = print + ":" + std::to_string(node->balance)+ ":" + std::to_string(node->left_index);
            std::cout << print << std::endl;

            // enter the next tree level - left and right branch
            print_tree( prefix + (isLeft ? "│   " : "    "), node->lchild, true);
            print_tree( prefix + (isLeft ? "│   " : "    "), node->rchild, false);
        }
    }

    void print_tree()
    {
        print_tree("", root, false);
    }

    static void print_leaf_contents(const node *node) {
        if((node->lchild == nullptr) && (node->rchild == nullptr)) {
            node->contents->print();
        } else {
            print_leaf_contents(node->lchild);
            print_leaf_contents(node->rchild);
        }
    }

    void print_leaf_contents() {
        print_leaf_contents(root);
    }
};

#endif //ORAM_POSMAP_COUNTER_INTERVAL_H

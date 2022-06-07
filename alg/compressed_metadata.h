//
// Created by wholland on 19/04/22.
//

#ifndef ORAM_POSMAP_COMPRESSED_METADATA_H
#define ORAM_POSMAP_COMPRESSED_METADATA_H

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


class compressed_metadata : public position_map
{
protected:
    node *root_counter;
    node *root_level;
    uint32_t width;

public:

    explicit compressed_metadata(uint32_t size, uint32_t width) :
        position_map(size),
        width(width)
    {
        // compressed frequency information
        root_counter = new node(0);
        root_counter->contents = new leaf_contents(width, 1, 0);

        // compressed level information
        root_level = new node(0);
        root_level->contents = new leaf_contents(width, 1, L);
    }

    ~compressed_metadata() {
        delete_tree(root_counter);
        delete_tree(root_level);
    }

    void add_address(address addr) override {

        // retrieve count and level (this is simulating what we would do)
        level_query(addr);

        // add level offsets during a rebuild
        uint64_t count = count_query(addr);
        // find leaf that contains address
        node *leaf = find_leaf(addr, root_counter);
        assert(leaf->contents != nullptr);
        leaf->contents->update_value(addr, count+1);

        if(leaf->check_split(width)) {
            leaf->split(width);
            height_change(leaf, INCREASE);
        } else if(leaf->check_merge(width)) {
            node* new_leaf = leaf->parent;
            new_leaf->merge(width);
            new_leaf->delete_siblings();
            height_change(new_leaf, DECREASE);
        }
    }

    void add_level(const address addr, uint32_t level) override {
        node *leaf = find_leaf(addr, root_level);
        assert(leaf->contents != nullptr);
        leaf->contents->update_value(addr, level);

        if(leaf->check_split(width)) {
            leaf->split(width);
            height_change(leaf, leaf->depth);
        } else if(leaf->check_merge(width)) {
            node* new_leaf = leaf->parent;
            new_leaf->merge(width);
            new_leaf->delete_siblings();
            height_change(new_leaf, new_leaf->depth);
        }
    }

    uint32_t auxiliary_info(address addr) override {
        return count_query(addr);
    }

    uint32_t level_query(address addr) override {
        node *leaf = find_leaf(addr, root_level);
        return leaf->contents->addr_count(addr);
    }

    uint32_t count_query(address addr) {
        node *leaf = find_leaf(addr, root_counter);
        return leaf->contents->addr_count(addr);
    }

    static node *find_leaf(address addr, node *curr_node) {
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
        } else {
            print_tree("", parent->parent, false);
        }
        node* grandparent = parent->parent;
        // check if a change in root occurred.
        if(root_counter == parent) {
            root_counter = child;
        } else if(root_level == parent) {
            root_level = child;
        } else {
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
        uint16_t depth = child->calc_depth();
        child->depth = depth;
        parent->depth = depth+1;
    }

    void height_change(node *prev, int depth) {
        node *curr = prev->parent;
        if(curr != nullptr) {
            // continue
            node *prev_sibling = prev->is_rchild ? (curr->lchild) : (curr->rchild);
            int other_depth = 0;
            if(prev_sibling != nullptr) {
                other_depth = prev_sibling->depth;
            }
            int balance = prev->is_rchild ? (depth-other_depth) : (other_depth-depth);

            if(balance > 1 || balance < -1) {
                node *tall_child, *X;
                switch(balance) {
                    case 2:
                        tall_child = curr->rchild;
                        switch(tall_child->calculate_balance()) {
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
                        switch(tall_child->calculate_balance()) {
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
            curr->depth = (depth > other_depth) ? (depth+1) : (other_depth+1);
            height_change(curr, curr->depth);
        }
    }

    // printing functions

    void static delete_tree(node *node) {
        if(node != nullptr) {
            delete_tree(node->lchild);
            delete_tree(node->rchild);
            delete node;
        }
    }

    static void print_tree(const std::string& prefix, const node *node, bool isLeft)
    {
        if( node != nullptr ) {
            std::cout << prefix;

            std::cout << (isLeft ? "├──" : "└──" );

            // print the value of the node
            uint16_t count = (node->contents == nullptr) ? 0 : node->contents->count();
            std::string print = std::to_string( count);
            print = print + ":" + std::to_string(node->depth)+ ":" + std::to_string(node->left_index);
            std::cout << print << std::endl;

            // enter the next tree level - left and right branch
            print_tree( prefix + (isLeft ? "│   " : "    "), node->lchild, true);
            print_tree( prefix + (isLeft ? "│   " : "    "), node->rchild, false);
        }
    }

    void print_tree_counter()
    {
        print_tree("", root_counter, false);
    }

    void print_tree_level()
    {
        printf("print tree\n");
        print_tree("", root_level, false);
    }

    static void print_leaf_contents(const node *node) {
        if((node->lchild == nullptr) && (node->rchild == nullptr)) {
            node->contents->print();
        } else {
            print_leaf_contents(node->lchild);
            print_leaf_contents(node->rchild);
        }
    }

    void print_leaf_contents(bool counter) {
        if(counter) {
            print_leaf_contents(root_counter);
        } else {
            print_leaf_contents(root_level);
        }
    }
};

#endif //ORAM_POSMAP_COMPRESSED_METADATA_H

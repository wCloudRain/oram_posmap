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
        add_value(&root_counter, addr, count+1);
        //print_tree_counter();
        //print_leaf_contents(true);
    }

    void add_level(const address addr, uint32_t level) override {
        add_value(&root_level, addr, level);
    }

    void add_value(node **root, const address addr, uint32_t value) {
        node *leaf = find_leaf(addr, *root);
        assert(leaf->contents != nullptr);
        leaf->contents->update_value(addr, value);

        if(leaf->check_split(width)) {
            leaf->split(width);
            //print_tree_level();
            //printf("split\n");
            *root = height_change(*root, leaf->left_index);

        } else {
            node *parent = leaf->parent;
            if(parent != nullptr) {
                if(parent->check_merge(width)) {
                    //printf("merge\n");
                    parent->merge(width);
                    parent->delete_siblings();
                    *root = height_change(*root, parent->left_index);
                }
            }
        }
        /*
        printf("\n::LEVEL::\n");
        print_tree_level();
        print_leaf_contents(false);
        print_tree_counter();

        */
        //print_tree_level();
        //print_leaf_contents(true);
    }

    static int height(node *head){
        if(head== nullptr) return 0;
        return head->height;
    }
    
    static node *right_rotation(node *head) {
        //rotate
        //printf("rotating right around %d\n", head->ID);
        node *new_head = head->lchild;
        head->lchild = new_head->rchild;
        new_head->rchild = head;

        // change parents
        new_head->parent = head->parent;
        head->parent = new_head;
        head->lchild->parent = head;

        // update heights
        head->height = 1+max(height(head->lchild), height(head->rchild));
        new_head->height = 1+max(height(new_head->lchild), height(new_head->rchild));

        // update left_index
        head->left_index = head->lchild->left_index;
        return new_head;
    }

    static node *left_rotation(node *head) {
        //printf("rotating left around %d\n", head->ID);
        // rotate
        node * new_head = head->rchild;
        head->rchild = new_head->lchild;
        new_head->lchild = head;

        // update parents
        new_head->parent = head->parent;
        head->parent = new_head;
        head->rchild->parent = head;

        // update heights
        head->height = 1+max(height(head->lchild), height(head->rchild));
        new_head->height = 1+max(height(new_head->lchild), height(new_head->rchild));

        // update left_index
        new_head->left_index = head->left_index;
        return new_head;
    }

    static node *balance(node *temp) {
        temp->height = 1 + max(height(temp->lchild), height(temp->rchild));
        int bal_factor = temp->calculate_balance();
        //printf("ID %d, bal factor %d\n", temp->ID, bal_factor);
        if (bal_factor > 1) {
            //printf("ID %d, bal factor %d\n", temp->ID, bal_factor);
            if (temp->rchild->calculate_balance() > 0) {
                //printf("rot left\n");
                return left_rotation(temp);
            } else {
                //printf("rot right-left\n");
                temp->rchild = right_rotation(temp->rchild);
                return left_rotation(temp);
            }
        } else if (bal_factor < -1) {
            //printf("ID %d, bal factor %d\n", temp->ID, bal_factor);
            if (temp->lchild->calculate_balance() > 0) {
                //printf("rot right\n");
                temp->lchild = left_rotation(temp->lchild);
                return right_rotation(temp);
            } else {
                //printf("rot left-right\n");
                return right_rotation(temp);
            }
        }
        return temp;
    }

    static node *height_change(node *root, uint64_t index) {
        //printf("node %d, searching for index %lu\n", root->ID, index);
        if((root->lchild == nullptr) && (root->rchild == nullptr)) {
            root->height = 1;
            return root;
        } else if (index < root->rchild->left_index) {
            root->lchild = height_change(root->lchild, index);
            return balance(root);
        }
        else {
            root->rchild = height_change(root->rchild, index);
            return balance(root);
        }
        //return root;
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
            print = print + ":" + std::to_string(node->height)+ ":" + std::to_string(node->left_index);
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

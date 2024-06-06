#include <iostream>
#include <assert.h>
#include <string>
#include "btree.h"

// A B-Tree has three parts: (i) Root Node, (ii) Internal Nodes, (iii) Leaf Nodes
// The root node has between 2 and m children
// Each internal node has between (m / 2) and m children. An internal node can have (m - 1) keys
// Each leaf node stores between ((m - 1) / 2) and (m - 1) keys
// The height of a b-tree can be yielded by the following:
// h <= log_m ((n + 1) / 2)
// m is the minimum degree and n is the number of keys

#define MAX_KEYS (1024)

// The most important thing is that a b-tree node contains "keys" and points to children.
struct node_t {
    bool leaf;
    int number_of_keys;
    int keys[MAX_KEYS];
    struct node_t * children[MAX_KEYS + 1];
};

class b_tree{
    private:
        struct node_t * root;
        int t;
    public:
        // Constructor and Destructor
        b_tree(int degree) {
            root = NULL;
            t = degree;
        }

        ~b_tree();

        void insert(int data) {
            // guard. if root is NULL insert at the root first
            if (root == NULL) {
                root = new node_t();
                root->leaf=true;
                root->keys[0]=data;
                root->number_of_keys=1;
            } else {
                find_node(data, root, t)
            }
        }

        void traverse() {}



};


int main(int argc, const char * argv[]) {
    return 0;
}

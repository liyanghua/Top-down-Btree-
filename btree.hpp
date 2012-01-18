#ifndef _TOP_DOWN_BTREE_HPP
#define _TOP_DOWN_BTREE_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <sstream>

#include <assert.h>
#include <stdio.h>

// for ceil and floor
#include <cmath>

using namespace std;

template<typename KeyType, typename ValType>
class BTreeNode {
    //everything makes public
    public:
        struct Element {
            KeyType m_key;
            ValType m_value;

            // for sorting and comparation
            bool operator >  (Element& other) const { return m_key >  other.m_key; }  
            bool operator <  (Element& other) const { return m_key <  other.m_key; }   
            bool operator >= (Element& other) const { return m_key >= other.m_key; }    
            bool operator <= (Element& other) const { return m_key <= other.m_key; }    
            bool operator == (Element& other) const { return m_key == other.m_key; }

            Element& operator= (const Element& m) {
                if (this != &m) {
                    m_key = m.m_key;
                    m_value = m.m_value;
                }

                return *this;
            }

            Element() {}

            Element(const Element& m) {
                m_key = m.m_key;
                m_value = m.m_value;
            }

            bool operator <   (KeyType& key) const { return m_key < key; }   
            bool operator >   (KeyType& key) const { return m_key > key; }
            bool operator ==  (KeyType& key) const { return m_key == key; }   
        };

        //  keys are in ordered keys[0] < keys[1] ... < keys[number_of_keys - 1]
        vector<Element> keys;
        // children.size() = keys.size() + 1 
        vector<BTreeNode*> children;

        // the parent
        BTreeNode* parent;

        bool is_leaf;

        int number_of_keys;

        BTreeNode() : parent(NULL), is_leaf(true), number_of_keys(0) {
            keys.clear();
            children.clear();
        }

        BTreeNode(int m) : parent(NULL), is_leaf(true), number_of_keys(0) {
            // allocate the memory in a greedy fashion
            keys.reserve(m-1);
            children.reserve(m);
        }
        
        // index from 0
        int size() { return number_of_keys; }
        void dump() {
            cout << "(";

            for (int i=0; i<size(); ++i) {
                printf("%c", keys[i].m_key);
                // cout << keys[i].m_key;
                if (i != size() - 1) {
                    cout << " ";
                }
            }
            cout << ")";
        }
};



template<typename KeyType, typename ValType>
class TopDownBTree {
    typedef BTreeNode<KeyType, ValType> NodeType;

    private:
        NodeType* root;
        // the order of the btree, max number of childrens. We use the Knuth's definition of 
        // the order.we define the order to be the maximum number of children.
        int m_order;

        NodeType* INVALID_NODE;

    public:
        TopDownBTree(int m) : root(new NodeType(m)), m_order(m) {
            INVALID_NODE = reinterpret_cast<NodeType*> (-1);
        }
        ~TopDownBTree() {
            delete root;
        }

        // return false if not FOUND, else return a refrence val to 
        // the value
        bool search(const KeyType& key, ValType& val);
        bool remove(const KeyType& key, ValType& val);
        bool insert(const KeyType& key, const ValType& val);

        void dump() {
            cout << "dump the tree...." << endl;

            queue<NodeType*> q;
            q.push(root);
            q.push(INVALID_NODE);

            vector<NodeType*> backup;


            for(;;) {
                backup.clear();

                while(!q.empty()) {
                    NodeType* n = q.front();
                    q.pop();

                    if (n == INVALID_NODE) {
                        cout << endl;
                    }
                    else {
                        n->dump();
                        backup.push_back(n);
                    }
                }
                
                bool has_node = false;
                size_t j = 0;
                while (j < backup.size()) {
                    NodeType* n = backup[j];
                    for (size_t i=0; i< n->children.size(); ++i) {
                        if (n->children[i] == NULL) continue;
                        q.push(n->children[i]);
                        has_node = true;
                    }
                    ++j;
                }

                if (has_node)
                    q.push(INVALID_NODE);
                else 
                    break;
            }

            cout << "dump OK..." << endl;
        }

    private:
        inline int lower_bound_of_keys() { return ceil(m_order/2) - 1; }
        inline int upper_bound_of_keys() { return m_order - 1; }

        // search in the subtree rooted x
        bool search_subtree(NodeType* x, const KeyType& key, ValType& val) {
            assert( x != NULL);

            int i = 0;
            while (i < x->size() && (*x)[i] < key) ++i;
            if (i < x->size() && ((*x)[i] == key)) {
                val = (*x)[i].value;
                cout << "FIND val: " << val << "for KEY: " << key << endl;

                return true;
            } 

            if (x->is_leaf()) { // we reach a leaf node and did not find
                cout << "Not FIND for key: " << key << endl;
                return false;
            }
            else { // we are in a internal node then search recursively
                assert( x->children[i] != NULL);
                return search_subtree(x->children[i], key, val);
            }
        }
        
        // insert key to a nonfull node x
        bool insert_nonfull_node(NodeType*& x, const KeyType& key, const ValType& val) {
            cout << "before insert, node size: " << x->size() << ", node capacity:" << x->keys.capacity() << endl;

            int i = x->size();
            // x should not be full
            assert(i < upper_bound_of_keys());

            if (x->is_leaf) {
                cout << "node is leaf.." << endl;


                int j = i-1;
                while (j >=0 && x->keys[j].m_key > key) {
                    x->keys[j+1] = x->keys[j];
                    --j;
                }
                // insert the new element
                x->keys[j+1].m_key = key;
                x->keys[j+1].m_value = val;

                x->number_of_keys += 1;

                cout << "Insert successfully!..." << endl;

                return true;
            }
            // search the right substree to insert
            int j = i-1;
            while (j >=0 && x->keys[j].m_key > key) {
                --j;
            }
            j++;
            cout << "search the " << j << " subtree" << endl;
            // full
            if (x->children[j]->size() == upper_bound_of_keys()) {
                // split it
                cout << "split..." << endl;
                split_node(x, j, x->children[j]);
                if (x->keys[j].m_key < key) ++j;
            }

            return insert_nonfull_node(x->children[j], key, val);

        }

        // split a btree node.
        // Before split: x is the parent of y, x[i] = y
        // and y is full.
        // After split, y splits into two nodes. A new node Z will be 
        // created and becomes the new child of X. x[i+1] = Z
        // this method will guarantee that recursion of insertion never descends
        // to a full node
        void split_node(NodeType*& x, int i, NodeType*& y) {
            // legality checking
            assert(y->size() == upper_bound_of_keys());
            assert(x != NULL && y != NULL && x->children[i] == y);

            NodeType* z = new NodeType(m_order);
            assert(z != NULL);

            // node z, y in the same level
            z->is_leaf = y->is_leaf;
            // right half of keys of y going to Z, including the middle element
            int t = (m_order - 1) / 2 + 1; 
            int to_move = m_order - 1 - t;
            cout << "move " << to_move << " keys to new node..." << endl;

            z->keys.reserve(to_move);
            z->children.reserve(to_move + 1);
            z->number_of_keys = to_move;
            
            // copy keys from y to z
            cout << "copy keys from y to z.." << endl;
            for (int j=0; j<to_move; ++j) {
                cout << "copy keys: " << y->keys[t+j].m_key << endl;
                z->keys.push_back(y->keys[t + j]);
            }

            cout << "now z is: " << endl;
            z->dump();
            
            // if y is not leaf, we need to adjust the points of the children
            if (!y->is_leaf) {
                for (int j=0; j<=to_move; ++j) {
                    cout << "adjust the " << j+t << "th pointers of y" << endl;
                    //z->children[j] = y->children[j + t];
                    z->children.push_back(y->children[j + t]);
                    y->children[j + t] = NULL;
                }
            }
            y->number_of_keys = t;

            // adjust the pointers of x
            //for (int j=x->size(); j>=i+1; j--) {
            //    cout << "adjust the " << j << "pointers of x.." << endl;
                // make space for the pointer to z
            //    x->children[j+1] = x->children[j];
            // }
            // put the new node z
            cout << "put the new node..." << endl;
            //x->children[i+1] = z;
            x->children.insert(x->children.begin() + i + 1, z);

            // put the middle of the key in y to x
            cout << "adjust keys of x.." << endl;
            for (int j=x->size() - 1; j>=i; j--) {
                x->keys[j+1] = x->keys[j];
            }
            // put the middle key of y
            cout << "put the middle key to x: " << y->keys[t-1].m_key << endl;
            x->keys[i] = y->keys[t-1];
            // erase it from y
            y->keys.erase(y->keys.begin() + t - 1);
            y->number_of_keys -= 1;

            // update the key number in x
            x->number_of_keys += 1;

            cout << "split OK.." << endl;
        }
};

//====implementation of top down btree.==========================================
template<typename KeyType, typename ValType>
bool TopDownBTree<KeyType, ValType>::search(const KeyType& key, ValType& val) {
    assert( root != NULL);

    return search_subtree(root, key, val);
}

template<typename KeyType, typename ValType>
bool TopDownBTree<KeyType, ValType>::remove(const KeyType& key, ValType& val) {
    return true;
}

// The core of insert is the node split when the number of keys in the active node
// >= 2*M - 1 where M is the order of the btree
template<typename KeyType, typename ValType>
bool TopDownBTree<KeyType, ValType>::insert(const KeyType& key, const ValType& val) {
    cout << "Insert key: " << key << ", val:" << val << endl;

    NodeType* r = root;
    if (r->size() == upper_bound_of_keys()) {
        cout << "Split root..." << endl;

        NodeType* s = new NodeType(m_order);
        assert(s != NULL);
        // set s as the new root
        root = s;
        s->is_leaf = false;
        s->children.push_back(r);
        r->parent = s;
        s->number_of_keys = 0;

        cout << "before split.." << endl;
        s->dump();
        r->dump();

        // split the node r
        split_node(s, 0, r);
        cout << "after split..." << endl;
        s->dump();
        r->dump();

        return insert_nonfull_node(s, key, val);
    }
    
    cout << "Insert to non full node..." << endl;

    return insert_nonfull_node(r, key, val);
}


#endif

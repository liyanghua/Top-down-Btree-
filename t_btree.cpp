#include <string>
#include "btree.hpp"

int main(int argc, char* argv[]) {
    // the max number of children of a node
    int btree_order = 5;
    TopDownBTree<int, int> btree(btree_order);

    // test insert: C N G A H E K Q M F W L T Z D P R X Y S

    string s("CNGAHEKQMFWLTZDPRXYS");
    // C N G A will be insert in a leaf directly
    for (size_t i=0; i<4; ++i) {
        btree.insert(s[i], s[i]);
        cout << "===============" << endl;
    }

    // this will cause a node split
    btree.insert(s[4], s[4]);

    btree.dump();
    
    // insert E, K, Q
    for(size_t i=5; i<8; ++i) {
        btree.insert(s[i], s[i]);
    }

    btree.dump();
    
    // insert M will cause a node split
    btree.insert(s[8], s[8]);
    btree.dump();

    // insert F,W,L,T,Z
    for(size_t i=9; i<14; ++i) {
        btree.insert(s[i], s[i]);
    }
    btree.dump();


    // insert D will cause a node split
    btree.insert(s[14], s[14]);
    btree.dump();

    // insert P will cause a node split
    btree.insert(s[15], s[15]);
    btree.dump();

    // insert R
    btree.insert(s[16], s[16]);
    btree.dump();

    // insert X
    btree.insert(s[17], s[17]);
    btree.dump();
        
    // insert Y
    btree.insert(s[18], s[18]);
    btree.dump();
    
    // insert S
    btree.insert(s[19], s[19]);
    btree.dump();


    return 0;

}

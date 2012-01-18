#include "btree.hpp"

 

int main(int argc, char* argv[])

{

// the main function is just some code to test the b-tree.  it inserts 100,000 elements,

// then searches for each of them, then deletes them in reverse order (also tested in

// forward order) and searches for all 100,000 elements after each deletion to ensure that

// all remaining elements remain accessible.

 
    Node::m_failure.invalidate();

    Node::m_failure.m_key = "";

    Elem elem;

 

    RootTracker tracker;  // maintains a pointer to the current root of the b-tree

    Node* root_ptr = new Node(tracker);

    tracker.set_root (null_ptr, root_ptr);

   

    vector<string> search_vect;

    // prepare the key strings

    search_vect.resize (100000);

    int search_count = 0;
    int i = 0;

    for (i=0; i<100000; i++) {

        strstream stm;

        stm << i;

        stm >> search_vect[search_count++];

    }

    string s;

    cout << "finished preparing key strings\n";


    for (i=0; i<100000; i++) {

        elem.m_key = search_vect[i];

        elem.m_payload = search_vect[i]+" hi you";

        tracker.get_root()->tree_insert(elem);

    }

    cout << "finished inserting elements\n";

    Node * last;

    for (i=0; i<100000; i++) {

        Elem desired;

        desired.m_key = search_vect[i];

        Elem& result = tracker.get_root()->search (desired, last);

    }

    cout << "finished searching for elements\n";

    for (i=99999; i>=0; i--) {

        Elem target;

        target.m_key = search_vect[i];

        tracker.get_root()->delete_element (target);

    }

    cout << "after deletion" << endl;

    tracker.get_root()->dump();


    return 0;

}

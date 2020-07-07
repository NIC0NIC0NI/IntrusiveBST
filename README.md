# IntrusiveBST
Implementation of intrusive binary search trees: Red-Black Tree, AVL and Weak AVL.

### Compile
Just `make`.

### Features

* The tree is intrusive. The user should define their own node types, allocate and deallocate the nodes. Polymorphic nodes are allowed. (see test/poly.cpp)

* Use 3 pointers to parent, left child and right child per node. The tag information is compressed in the 2 low bits of the parent pointer, assuming the nodes are at least 4-byte aligned. For RB-Tree, the tag is the color; for AVL, the tag has 3 states: left higher, balanced and right higher; for WAVL there are 4 states: left higher, balanced, right higher, and an additional state representing 2,2-nodes in the WAVL paper. A 2,2-node is balanced, and When counting the height, a 2,2-node is counted as 2 nodes in height.

* `search` and `insert` are templates because it has to call the user-defined comparison function. Post-insert rebalancing, `erase` and iterating are not templates for smaller code size.

* Although the iterator is bidirectional, the end sentinel is represented by `nullptr`. Once the iterator moves to the next of the last element, it cannot move back.
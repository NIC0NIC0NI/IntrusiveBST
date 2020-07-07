# IntrusiveBST
Implementation of intrusive binary search trees: Red-Black tree, AVL and WAVL.

Features:

* The tree is intrusive. The user should define their own node types, allocate and deallocate the nodes. Polymorphic nodes are allowed (see test/poly.cpp)

* Use 3 pointers to parent, left child and right child per node. The tag information is compressed in the 2 low bits of the parent pointer, assuming the nodes are at least 4-byte aligned. For RB-Tree, the tag is the color; for AVL, the tag has 3 states: left higher, balanced and right higher; for WAVL, an additional state means this node is balanced and counted as 2 nodes in height. (2,2-node in the WAVL paper)

* `search` and `insert` are templates because it has to call the user-defined comparison function. Post-insert rebalancing, `erase` and iterating are not templates for smaller code size.

Although the iterator is bidirenctional, the end sentinel is represented by `nullptr`. Once the iterator moves to the next of the last element, it cannot move back.
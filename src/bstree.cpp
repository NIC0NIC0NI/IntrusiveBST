#include"bstree.h"

/* The red-black tree code is modified from linux kernel
 */

namespace bst {
namespace impl {

using Node = NodeBase;

#define RED     0
#define BLACK   3
#define LEFT    3 // left  higher
#define RIGHT   2 // right higher
#define BALANCE 0
#define WEAK    3
#define WLEFT   1
#define WRIGHT  2


inline void replace_node_as_left_child(Node* newnode, Node* parent) {
    parent->left = newnode;
}
inline void replace_node_as_right_child(Node* newnode, Node* parent) {
    parent->right = newnode;
}

inline void replace_node(Node* oldnode, Node* newnode, Node* parent, Node*& root) {
    if (parent) {
        if (parent->left == oldnode) {
            replace_node_as_left_child(newnode, parent);
        } else  {
            replace_node_as_right_child(newnode, parent);
        }
    } else {
        root = newnode;
    }
}


inline void rotate_left_as_left_child(Node* node) {
    auto right = node->right;
    auto parent = node->parent();
    node->right = right->left;
    if (right->left)
        right->left->set_parent(node);
    right->left = node;
    right->set_parent(parent);
    replace_node_as_left_child(right, parent);
    node->set_parent(right);
}

inline void rotate_right_as_right_child(Node* node) {
    auto left = node->left;
    auto parent = node->parent();
    node->left = left->right;
    if (left->right) 
        left->right->set_parent(node);
    left->right = node;
    left->set_parent(parent);
    replace_node_as_right_child(left, parent);
    node->set_parent(left);
}

inline void rotate_left(Node* node, Node*& root) {
    auto right = node->right;
    auto parent = node->parent();
    node->right = right->left;
    if (right->left)
        right->left->set_parent(node);
    right->left = node;
    right->set_parent(parent);
    replace_node(node, right, parent, root);
    node->set_parent(right);
}

inline void rotate_right(Node* node, Node*& root) {
    auto left = node->left;
    auto parent = node->parent();
    node->left = left->right;
    if (left->right) 
        left->right->set_parent(node);
    left->right = node;
    left->set_parent(parent);
    replace_node(node, left, parent, root);
    node->set_parent(left);
}

Node* rb_post_insert(Node* node, Node* root) {
    Node *parent, *gparent;
    node->set_tag<RED>();

    while ((parent = node->parent()) && parent->tag() == RED) {
        gparent = parent->parent();

        if (parent == gparent->left) {
            {
                Node *uncle = gparent->right;
                if (uncle && uncle->tag() == RED) {
                    uncle->set_tag<BLACK>();
                    parent->set_tag<BLACK>();
                    gparent->set_tag<RED>();
                    node = gparent;
                    continue;
                }
            }

            if (parent->right == node) {
                Node *tmp;
                rotate_left_as_left_child(parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            parent->set_tag<BLACK>();
            gparent->set_tag<RED>();
            rotate_right(gparent, root);
        } else {
            {
                Node *uncle = gparent->left;
                if (uncle && uncle->tag() == RED) {
                    uncle->set_tag<BLACK>();
                    parent->set_tag<BLACK>();
                    gparent->set_tag<RED>();
                    node = gparent;
                    continue;
                }
            }

            if (parent->left == node) {
                Node *tmp;
                rotate_right_as_right_child(parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            parent->set_tag<BLACK>();
            gparent->set_tag<RED>();
            rotate_left(gparent, root);
        }
    }

    root->set_tag<BLACK>();
    return root;
}

inline Node *rb_post_erase(Node *node, Node *parent, Node *root) {
    Node *other;

    while ((!node || node->tag() == BLACK) && node != root) {
        if (parent->left == node) {
            other = parent->right;
            if (other->tag() == RED) {
                other->set_tag<BLACK>();
                parent->set_tag<RED>();
                rotate_left(parent, root);
                other = parent->right;
            }
            if ((!other->left || other->left->tag() == BLACK) && (!other->right || other->right->tag() == BLACK)) {
                other->set_tag<RED>();
                node = parent;
                parent = node->parent();
            } else {
                if (!other->right || other->right->tag() == BLACK) {
                    Node *o_left;
                    if ((o_left = other->left))
                        o_left->set_tag<BLACK>();
                    other->set_tag<RED>();
                    rotate_right_as_right_child(other);
                    other = parent->right;
                }
                other->set_tag(parent->tag());
                parent->set_tag<BLACK>();
                if (other->right)
                    other->right->set_tag<BLACK>();
                rotate_left(parent, root);
                node = root;
                break;
            }
        } else {
            other = parent->left;
            if (other->tag() == RED) {
                other->set_tag<BLACK>();
                parent->set_tag<RED>();
                rotate_right(parent, root);
                other = parent->left;
            }
            if ((!other->left || other->left->tag() == BLACK) && (!other->right || other->right->tag() == BLACK)) {
                other->set_tag<RED>();
                node = parent;
                parent = node->parent();
            } else {
                if (!other->left || other->left->tag() == BLACK) {
                    Node *o_right;
                    if ((o_right = other->right))
                        o_right->set_tag<BLACK>();
                    other->set_tag<RED>();
                    rotate_left_as_left_child(other);
                    other = parent->left;
                }
                other->set_tag(parent->tag());
                parent->set_tag<BLACK>();
                if (other->left)
                    other->left->set_tag<BLACK>();
                rotate_right(parent, root);
                node = root;
                break;
            }
        }
    }
    if (node)
        node->set_tag<BLACK>();
    return root;
}

Node* avl_post_insert(Node* node, Node* root) {
    node->set_tag<BALANCE>();
    for (Node* parent = node->parent(); parent; node = parent, parent = node->parent()) {
        auto tag = parent->tag();
        if(node == parent->left) { // left child
            if(tag == LEFT) {
                auto node_tag = node->tag();
                if(node_tag == RIGHT) {
                    auto tmp = node->right;
                    auto tmp_tag = tmp->tag();
                    rotate_left_as_left_child(node);
                    parent->set_tag((tmp_tag == LEFT) ? RIGHT : BALANCE);
                    node->set_tag((tmp_tag == RIGHT) ? LEFT : BALANCE);
                    tmp->set_tag<BALANCE>();
                } else {
                    parent->set_tag((node_tag == LEFT) ? BALANCE : LEFT);
                    node->set_tag((node_tag == LEFT) ? BALANCE : RIGHT);
                }

                rotate_right(parent, root);
                return root;

            } else if (tag == BALANCE) {
                parent->set_tag<LEFT>();

            } else {
                parent->set_tag<BALANCE>();
                return root;
            }
        } else {                   // right child
            if(tag == RIGHT) {
                auto node_tag = node->tag();
                if(node_tag == LEFT) {
                    auto tmp = node->left;
                    auto tmp_tag = tmp->tag();
                    rotate_right_as_right_child(node);
                    parent->set_tag((tmp_tag == RIGHT) ? LEFT : BALANCE);
                    node->set_tag((tmp_tag == LEFT) ? RIGHT : BALANCE);
                    tmp->set_tag<BALANCE>();
                } else {
                    parent->set_tag((node_tag == RIGHT) ? BALANCE : RIGHT);
                    node->set_tag((node_tag == RIGHT) ? BALANCE : LEFT);
                }

                rotate_left(parent, root);
                return root;

            } else if (tag == BALANCE) {
                parent->set_tag<RIGHT>();

            } else {
                parent->set_tag<BALANCE>();
                return root;
            }
        }
    }
    return root;
}

inline Node* avl_post_erase(Node* node, Node *parent, Node* root, bool left_child) {
    for(;;) {
        auto tag = parent->tag();
        if(left_child) { // left child
            if(tag == RIGHT) {
                auto sibling = parent->right;
                auto sibling_tag = sibling->tag();
                if(sibling_tag == LEFT) {
                    auto tmp = sibling->left;
                    auto tmp_tag = tmp->tag();
                    rotate_right_as_right_child(sibling);
                    parent->set_tag((tmp_tag == RIGHT) ? LEFT : BALANCE);
                    sibling->set_tag((tmp_tag == LEFT) ? RIGHT : BALANCE);
                    tmp->set_tag<BALANCE>();
                    node = tmp;
                } else {
                    parent->set_tag((sibling_tag == BALANCE) ? RIGHT : BALANCE);
                    sibling->set_tag((sibling_tag == BALANCE) ? LEFT : BALANCE);
                    node = sibling;
                }

                rotate_left(parent, root);
                if(sibling_tag == BALANCE) {
                    return root;
                }

            } else if (tag == BALANCE) {
                parent->set_tag<RIGHT>();
                return root;

            } else {
                parent->set_tag<BALANCE>();
                node = parent;
            }
        } else {                   // right child
            if(tag == LEFT) {
                auto sibling = parent->left;
                auto sibling_tag = sibling->tag();
                if(sibling_tag == RIGHT) {
                    auto tmp = sibling->right;
                    auto tmp_tag = tmp->tag();
                    rotate_left_as_left_child(sibling);
                    parent->set_tag((tmp_tag == LEFT) ? RIGHT : BALANCE);
                    sibling->set_tag((tmp_tag == RIGHT) ? LEFT : BALANCE);
                    tmp->set_tag<BALANCE>();
                    node = tmp;
                } else {
                    parent->set_tag((sibling_tag == BALANCE) ? LEFT : BALANCE);
                    sibling->set_tag((sibling_tag == BALANCE) ? RIGHT : BALANCE);
                    node = sibling;
                }

                rotate_right(parent, root);
                if(sibling_tag == BALANCE) {
                    return root;
                }

            } else if (tag == BALANCE) {
                parent->set_tag<LEFT>();
                return root;

            } else {
                parent->set_tag<BALANCE>();
                node = parent;
            }
        }
        parent = node->parent();
        if(parent == nullptr) {
            break;
        }
        left_child = (node == parent->left);
    }
    return root;
}

Node* wavl_post_insert(Node* node, Node* root) {
    node->set_tag<BALANCE>();
    for (Node* parent = node->parent(); parent; node = parent, parent = node->parent()) {
        auto tag = parent->tag();
        if(node == parent->left) { // left child
            if(tag == WLEFT) {
                auto node_tag = node->tag();
                if(node_tag == WRIGHT) {
                    auto tmp = node->right;
                    auto tmp_tag = tmp->parent_with_tag;
                    rotate_left_as_left_child(node);
                    parent->set_tag(((tmp_tag & WLEFT) != 0) ? WRIGHT : BALANCE);
                    node->set_tag(((tmp_tag & WRIGHT) != 0) ? WLEFT : BALANCE);
                    tmp->set_tag<BALANCE>();
                } else {
                    parent->set_tag(((node_tag & WLEFT) != 0) ? BALANCE : WLEFT);
                    node->set_tag((node_tag == WLEFT) ? BALANCE : WRIGHT);
                }

                rotate_right(parent, root);
                return root;

            } else if (tag == WRIGHT) {
                parent->set_tag<BALANCE>();
                return root;

            } else  {
                parent->set_tag<WLEFT>();
                if (tag == WEAK) {
                    return root;
                }
            }
        } else {                   // right child
            if(tag == WRIGHT) {
                auto node_tag = node->tag();
                if(node_tag == WLEFT) {
                    auto tmp = node->left;
                    auto tmp_tag = tmp->parent_with_tag;
                    rotate_right_as_right_child(node);
                    parent->set_tag(((tmp_tag & WRIGHT) != 0) ? WLEFT : BALANCE);
                    node->set_tag(((tmp_tag & WLEFT) != 0) ? WRIGHT : BALANCE);
                    tmp->set_tag<BALANCE>();
                } else {
                    parent->set_tag(((node_tag & WRIGHT) != 0) ? BALANCE : WRIGHT);
                    node->set_tag((node_tag == WRIGHT) ? BALANCE : WLEFT);
                }

                rotate_left(parent, root);
                return root;

            } else if (tag == WLEFT) {
                parent->set_tag<BALANCE>();
                return root;

            } else {
                parent->set_tag<WRIGHT>();
                if (tag == WEAK) {
                    return root;
                }
            }
        }
    }
    return root;
}

inline Node* wavl_post_erase(Node* node, Node *parent, Node* root, bool left_child) {
    for(;;) {
        auto tag = parent->tag();
        if(left_child) { // left child
            if(tag == WRIGHT) {
                auto sibling = parent->right;
                auto sibling_tag = sibling->tag();

                if(sibling_tag == WEAK) {
                    sibling->set_tag<BALANCE>();

                } else {
                    if(sibling_tag == WLEFT) {
                        auto tmp = sibling->left;
                        auto tmp_tag = tmp->tag();
                        rotate_right_as_right_child(sibling);
                        parent->set_tag(((tmp_tag & WRIGHT) != 0) ? WLEFT : BALANCE);
                        sibling->set_tag(((tmp_tag & WLEFT) != 0) ? WRIGHT : BALANCE);
                        tmp->set_tag<WEAK>();
                    } else {
                        parent->set_tag((sibling_tag == WRIGHT) ? BALANCE : WRIGHT);
                        sibling->set_tag((sibling_tag == WRIGHT) ? WEAK : WLEFT);
                    }

                    rotate_left(parent, root);
                    return root;
                }

            } else if (tag == WLEFT) {
                parent->set_tag<BALANCE>();

            } else {
                parent->set_tag<WRIGHT>();
                if (tag == BALANCE) {
                    return root;
                }
            }
        } else {                   // right child
            if(tag == WLEFT) {
                auto sibling = parent->left;
                auto sibling_tag = sibling->tag();

                if(sibling_tag == WEAK) {
                    sibling->set_tag<BALANCE>();

                } else {
                    if(sibling_tag == WRIGHT) {
                        auto tmp = sibling->right;
                        auto tmp_tag = tmp->tag();
                        rotate_left_as_left_child(sibling);
                        parent->set_tag(((tmp_tag & WLEFT) != 0) ? WRIGHT : BALANCE);
                        sibling->set_tag(((tmp_tag & WRIGHT) != 0) ? WLEFT : BALANCE);
                        tmp->set_tag<WEAK>();
                    } else {
                        parent->set_tag((sibling_tag == WLEFT) ? BALANCE : WLEFT);
                        sibling->set_tag((sibling_tag == WLEFT) ? WEAK : WRIGHT);
                    }

                    rotate_right(parent, root);
                    return root;
                }
                
            } else if (tag == WRIGHT) {
                parent->set_tag<BALANCE>();

            } else {
                parent->set_tag<WLEFT>();
                if (tag == BALANCE) {
                    return root;
                }
            }
        }
        node = parent;
        parent = node->parent();
        if(parent == nullptr) {
            break;
        }
        left_child = (node == parent->left);
    }
    return root;
}


template<typename PostErase>
inline Node* bst_erase(Node *node, Node* root, PostErase post_erase) {
    Node *child, *parent;
    if (node->left && node->right) {
        Node *old = node, *tmp;
        node = node->right;
        while ((tmp = node->left) != nullptr) {
            node = tmp;
        }
        child = node->right;
        parent = node->parent();
        post_erase.set_color(node->tag());
        if (child) {
            child->set_parent(parent);
        }
        if (parent == old) {
            replace_node_as_right_child(child, parent);
            parent = node;
            post_erase.set_as_left_child(false);
        } else {
            replace_node_as_left_child(child, parent);
            post_erase.set_as_left_child(true);
        }

        tmp = old->parent();
        node->left = old->left;
        node->right = old->right;
        node->set_parent(tmp);
        node->set_tag(old->tag());
        replace_node(old, node, tmp, root);
        old->left->set_parent(node);
        if (old->right) {
            old->right->set_parent(node);
        }
    }
    else {
        if (node->left == nullptr) 
            child = node->right;
        else
            child = node->left;
        parent = node->parent();
        post_erase.set_color(node->tag());
        post_erase.set_as_left_child((parent != nullptr) && (parent->left == node));
        replace_node(node, child, parent, root);
        
        if (child) {
            child->set_parent(parent);
        }
    }
    return post_erase(child, parent, root);
}

Node* rb_erase(Node* node, Node* root) {
    struct {
        int color;
        void set_color(int c) { color = c; }
        void set_as_left_child(bool) const {}
        Node* operator()(Node* child, Node* parent, Node* root) {
            if(color == BLACK) {
                return rb_post_erase(child, parent, root);
            } else {
                return root;
            }
        }
    } post_erase;

    return bst_erase(node, root, post_erase);
}

Node* avl_erase(Node* node, Node* root) {
    struct {
        bool flag;
        void set_color(int) const {}
        void set_as_left_child(bool f) {
            flag = f;
        }
        Node* operator()(Node* child, Node* parent, Node* root) {
            if(parent != nullptr) {
                return avl_post_erase(child, parent, root, flag);
            } else {
                return root;
            }
        }
    } post_erase;

    return bst_erase(node, root, post_erase);
}

Node* wavl_erase(Node* node, Node* root) {
    struct {
        bool flag;
        void set_color(int) const {}
        void set_as_left_child(bool f) {
            flag = f;
        }
        Node* operator()(Node* child, Node* parent, Node* root) {
            if(parent != nullptr) {
                return wavl_post_erase(child, parent, root, flag);
            } else {
                return root;
            }
        }
    } post_erase;

    return bst_erase(node, root, post_erase);
}

Node* bst_first(Node* root) {
    auto p = root;
    if (p == nullptr)
        return nullptr; // empty
    while (p->left)
        p = p->left;
    return p;
}

Node* bst_last(Node* root) {
    auto p = root;
    if (p == nullptr)
        return nullptr; // empty
    while (p->right)
        p = p->right;
    return p;
}

Node* bst_next(Node* node) {
    if (node->right) {
        node = node->right; 
        while (node->left)
            node=node->left;
        return node;
    }
    
    while (node->parent() && node == node->parent()->right)
        node = node->parent();
    return node->parent();
}

Node* bst_prev(Node* node) {
    if (node->left) {
        node = node->left; 
        while (node->right)
            node=node->right;
        return node;
    }

    while (node->parent() && node == node->parent()->left)
        node = node->parent();

    return node->parent();
}

}
}
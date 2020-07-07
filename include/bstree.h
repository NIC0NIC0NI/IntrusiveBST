#ifndef BSTREE_H
#define BSTREE_H

#include<cstdint>
#include<iterator>
#include<type_traits>

namespace bst {
namespace impl {

struct NodeBase {
    using UP = std::uintptr_t;
    UP parent_with_tag;
    NodeBase* left;
    NodeBase* right;
    NodeBase* parent() const {
        return reinterpret_cast<NodeBase*>(parent_with_tag & ~static_cast<UP>(3));
    }
    void set_parent(NodeBase* p) {
        parent_with_tag &= static_cast<UP>(3);
        parent_with_tag |= reinterpret_cast<UP>(p);
    }
    int tag() const {
        return parent_with_tag & static_cast<UP>(3);
    }
    void set_tag(int t) {
        parent_with_tag &= ~static_cast<UP>(3);
        parent_with_tag |= static_cast<UP>(t);
    }
    void set_tag(std::integral_constant<int, 3>) {
        parent_with_tag |= static_cast<UP>(3);
    }
    void set_tag(std::integral_constant<int, 0>) {
        parent_with_tag &= ~static_cast<UP>(3);
    }
    template<int t>
    void set_tag() {
        set_tag(std::integral_constant<int, t>());
    }
};

extern NodeBase* bst_first(NodeBase* node);
extern NodeBase* bst_last(NodeBase* node);
extern NodeBase* bst_prev(NodeBase* node);
extern NodeBase* bst_next(NodeBase* node);
extern NodeBase* rb_post_insert(NodeBase* node, NodeBase* root);
extern NodeBase* avl_post_insert(NodeBase* node, NodeBase* root);
extern NodeBase* wavl_post_insert(NodeBase* node, NodeBase* root);
extern NodeBase* rb_erase(NodeBase* node, NodeBase* root);
extern NodeBase* avl_erase(NodeBase* node, NodeBase* root);
extern NodeBase* wavl_erase(NodeBase* node, NodeBase* root);

template<typename Left, typename Right>
class Tuple : Left {
    Right rr;
public:
    template<typename ... Args>
    Tuple(const Left& l, Args && ... args) : Left(l), rr(std::forward<Args>(args)...) {}
    Left& left() { return *this; }
    Right& right() { return this->rr; }
    const Left& left() const { return *this; }
    const Right& right() const { return this->rr; }
};

template<typename NodeType, typename Key, typename GetKey, typename Compare>
class bstree {
    static_assert(std::is_convertible<NodeType*, NodeBase*>::value, "The node type is not a subclass of node_hook");
    Tuple<GetKey, Tuple<Compare, NodeBase*>> data;
public:
    using node_type = NodeType;
    using node_pointer = node_type*;
    using compare = Compare;
    using value_type = Key;

    node_pointer first() const {
        return static_cast<node_pointer>(bst_first(this->root()));
    }
    node_pointer last() const {
        return static_cast<node_pointer>(bst_last(this->root()));
    }
    node_pointer root() const {
        return static_cast<node_pointer>(this->data.right().right());
    }

    bstree(const GetKey& key, const Compare& comp) : data(key, comp, nullptr) {}

    node_pointer search(const Key& value) const {
        auto& key = this->data.left();
        auto& comp = this->data.right().left();
        auto p = static_cast<node_pointer>(this->root());
        while(p != nullptr) {
            if(comp(value, key(*p))) {
                p = static_cast<node_pointer>(p->left);
            } else if(comp(key(*p), value)) {
                p = static_cast<node_pointer>(p->right);
            } else return p;
        }
        return nullptr;
    }
    
    node_pointer lower_bound(const Key& value) const {
        return lower_bound_impl(value, root(), this->data.right().left());
    }

    node_pointer upper_bound(const Key& value) const {
        auto& comp = this->data.right().left();
        return lower_bound_impl(value, root(), [&](const Key& l, const Key& r) { return !comp(r, l); });
    }

    // find nodes that 
    std::pair<node_pointer, node_pointer> search_range(const Key& lower, const Key& upper) const {
        auto& key = this->data.left();
        auto& comp = this->data.right().left();
        auto p = static_cast<node_pointer>(this->root());
        while(p != nullptr) {
            if(comp(lower, key(*p)) && comp(upper, key(*p))) {
                p = static_cast<node_pointer>(p->left);
            } else if(comp(key(*p), lower) && comp(key(*p), upper)) {
                p = static_cast<node_pointer>(p->right);
            } else return {lower_bound_impl(lower, p, comp), lower_bound_impl(upper, p, comp)};
        }
        return {nullptr, nullptr};

    }

protected:
    template<typename COMP>
    node_pointer lower_bound_impl(const Key& x, node_pointer p, COMP&& comp) const {
        auto& key = this->data.left();
        node_pointer q = nullptr;
        bool last_dir = false;
        while(p != nullptr) {
            q = p;
            last_dir = comp(key(*p), x);
            if(last_dir) {
                p = static_cast<node_pointer>(p->right);
            } else {
                p = static_cast<node_pointer>(p->left);
            }
        }
        if(last_dir) {
            return static_cast<node_pointer>(bst_next(q));
        }
        return q;
    }

    void insert_bst(node_pointer node) {
        auto& key = this->data.left();
        auto& comp = this->data.right().left();
        auto link = &(this->data.right().right());
        node_pointer parent = nullptr;
        while(*link != nullptr) {
            parent = static_cast<node_pointer>(*link);
            if(comp(key(*node), key(*parent))) { // allow duplicate
                link = &(parent->left);
            } else {
                link = &(parent->right);
            }
        }
        node->left = node->right = nullptr;
        node->set_parent(parent);
        *link = node;
    }

    bool insert_unique_bst(node_pointer node) {
        auto& key = this->data.left();
        auto& comp = this->data.right().left();
        auto link = &(this->data.right().right());
        node_pointer parent = nullptr;
        while(*link != nullptr) {
            parent = static_cast<node_pointer>(*link);
            if(comp(key(*node), key(*parent))) { // not allow duplicate
                link = &(parent->left);
            } else if(comp(key(*parent), key(*node))) {
                link = &(parent->right);
            } else return false;
        }
        node->left = node->right = nullptr;
        node->set_parent(parent);
        *link = node;
        return true;
    }

    void set_root(NodeBase* r) {
        this->data.right().right() = r;
    }
};

}

template<typename NodeType, typename Key, typename GetKey, typename Compare = std::less<Key>>
class rbtree : public impl::bstree<NodeType, Key, GetKey, Compare> {
    using Base = impl::bstree<NodeType, Key, GetKey, Compare>;
public:
    using node_type = NodeType;
    using node_pointer = node_type*;
    using compare = Compare;
    using value_type = Key;

    rbtree(const GetKey& key = GetKey(), const Compare& comp = Compare()) : Base(key, comp) {}
    rbtree(const Compare& comp) : Base(GetKey(), comp) {}

    void insert(node_pointer node) {
        this->insert_bst(node);
        this->set_root(rb_post_insert(node, this->root()));
    }
    void insert_unique(node_pointer node) {
        if (this->insert_unique_bst(node)) {
            this->set_root(rb_post_insert(node, this->root()));
        }
    }
    void erase(node_pointer node) {
        this->set_root(rb_erase(node, this->root()));
    }
};

template<typename NodeType, typename Key, typename GetKey, typename Compare = std::less<Key>>
class avl : public impl::bstree<NodeType, Key, GetKey, Compare> {
    using Base = impl::bstree<NodeType, Key, GetKey, Compare>;
public:
    using node_type = NodeType;
    using node_pointer = node_type*;
    using compare = Compare;
    using value_type = Key;

    avl(const GetKey& key = GetKey(), const Compare& comp = Compare()) : Base(key, comp) {}
    avl(const Compare& comp) : Base(GetKey(), comp) {}

    void insert(node_pointer node) {
        this->insert_bst(node);
        this->set_root(avl_post_insert(node, this->root()));
    }
    void insert_unique(node_pointer node) {
        if (this->insert_unique_bst(node)) {
            this->set_root(avl_post_insert(node, this->root()));
        }
    }
    void erase(node_pointer node) {
        this->set_root(avl_erase(node, this->root()));
    }
};

template<typename NodeType, typename Key, typename GetKey, typename Compare = std::less<Key>>
class wavl : public impl::bstree<NodeType, Key, GetKey, Compare> {
    using Base = impl::bstree<NodeType, Key, GetKey, Compare>;
public:
    using node_type = NodeType;
    using node_pointer = node_type*;
    using compare = Compare;
    using value_type = Key;

    wavl(const GetKey& key = GetKey(), const Compare& comp = Compare()) : Base(key, comp) {}
    wavl(const Compare& comp) : Base(GetKey(), comp) {}

    void insert(node_pointer node) {
        this->insert_bst(node);
        this->set_root(wavl_post_insert(node, this->root()));
    }
    void insert_unique(node_pointer node) {
        if (this->insert_unique_bst(node)) {
            this->set_root(wavl_post_insert(node, this->root()));
        }
    }
    void erase(node_pointer node) {
        this->set_root(wavl_erase(node, this->root()));
    }
};

namespace iter {
    
template<typename NodeType>
class Iterator {
    NodeType* NodePtr;
public:
    using difference_type = std::ptrdiff_t;
    using value_type = NodeType;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;
    explicit Iterator(NodeType* c) : NodePtr(c) {}
    Iterator(const Iterator& c) : NodePtr(c.NodePtr) {}

    Iterator& operator++() {
        NodePtr = static_cast<NodeType*>(bst_next(NodePtr));
        return *this;
    }
    Iterator& operator--() {
        NodePtr = static_cast<NodeType*>(bst_prev(NodePtr));
        return *this;
    }
    Iterator operator++(int) {
        Iterator res (*this);
        ++(*this);
        return res;
    }
    Iterator operator--(int) {
        Iterator res (*this);
        --(*this);
        return res;
    }
    bool operator==(const Iterator& other) const { return NodePtr == other.NodePtr; }
    bool operator!=(const Iterator& other) const { return NodePtr != other.NodePtr; }
    bool operator==(std::nullptr_t) const { return NodePtr == nullptr; }
    bool operator!=(std::nullptr_t) const { return NodePtr != nullptr; }
    reference operator*() const { return *NodePtr; }
    pointer operator->() const { return NodePtr; }
};


template<typename NodeType>
class ReverseIterator {
    NodeType* NodePtr;
public:
    using difference_type = std::ptrdiff_t;
    using value_type = NodeType;
    using pointer = value_type*;
    using reference = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit ReverseIterator(NodeType* c) : NodePtr(c) {}
    ReverseIterator(const ReverseIterator& c) : NodePtr(c.NodePtr) {}

    ReverseIterator& operator++() {
        NodePtr = static_cast<NodeType*>(bst_prev(NodePtr));
        return *this;
    }
    ReverseIterator& operator--() {
        NodePtr = static_cast<NodeType*>(bst_next(NodePtr));
        return *this;
    }
    ReverseIterator operator++(int) {
        ReverseIterator res (*this);
        ++(*this);
        return res;
    }
    ReverseIterator operator--(int) {
        ReverseIterator res (*this);
        --(*this);
        return res;
    }
    bool operator==(const ReverseIterator& other) const { return NodePtr == other.NodePtr; }
    bool operator!=(const ReverseIterator& other) const { return NodePtr != other.NodePtr; }
    bool operator==(std::nullptr_t) const { return NodePtr == nullptr; }
    bool operator!=(std::nullptr_t) const { return NodePtr != nullptr; }
    reference operator*() const { return *NodePtr; }
    pointer operator->() const { return NodePtr; }
};

template<typename Iter>
class FullRange {
    Iter first;
public:
    explicit FullRange(const Iter& i) : first(i) {}
    FullRange(const FullRange& r) : first(r.first) {}
    Iter begin() const {
        return first;
    }
    Iter end() const {
        return Iter(nullptr);
    }
};

template<typename Iter>
class Range : public FullRange<Iter> {
    Iter last;
public:
    explicit Range(Iter first, Iter last_) : FullRange<Iter>(first), last(last_) {}
    Range(const Range& r) : FullRange<Iter>(r), last(r.last) {}
    Iter end() const {
        return last;
    }
};
}


using node_hook = impl::NodeBase;

template<typename BST>
inline iter::FullRange<iter::Iterator<typename BST::node_type>> range(BST& bst) {
    using it = iter::Iterator<typename BST::node_type>;
    return iter::FullRange<it>(it(bst.first()));
}

template<typename BST>
inline iter::FullRange<iter::Iterator<typename BST::node_type>> crange(const BST& bst) {
    using it = iter::Iterator<const typename BST::node_type>;
    return iter::FullRange<it>(it(bst.first()));
}

template<typename BST>
inline iter::FullRange<iter::ReverseIterator<typename BST::node_type>> rrange(BST& bst) {
    using it = iter::ReverseIterator<typename BST::node_type>;
    return iter::FullRange<it>(it(bst.last()));
}

template<typename BST>
inline iter::FullRange<iter::ReverseIterator<typename BST::node_type>> crrange(const BST& bst) {
    using it = iter::ReverseIterator<const typename BST::node_type>;
    return iter::FullRange<it>(it(bst.last()));
}

template<typename NodeType>
inline iter::Range<iter::Iterator<NodeType>> range(NodeType* first, NodeType* last) {
    typedef iter::Iterator<NodeType> it;
    return iter::Range<it>(it(first), it(last));
}

template<typename NodeType>
inline iter::Range<iter::Iterator<const NodeType>> crange(NodeType* first, NodeType* last) {
    typedef iter::Iterator<const NodeType> it;
    return iter::Range<it>(it(first), it(last));
}

template<typename NodeType>
inline iter::Range<iter::Iterator<NodeType>> range(const std::pair<NodeType*,NodeType*>& r) {
    return range(r.first, r.second);
}

template<typename NodeType>
inline iter::Range<iter::Iterator<const NodeType>> crange(const std::pair<NodeType*,NodeType*>& r) {
    return crange(r.first, r.second);
}

}
#endif



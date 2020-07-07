#include<random>
#include<algorithm>
#include<iostream>
#include<iomanip>
#include<sys/time.h>
#include"bstree.h"

struct PolyNode : public bst::node_hook {
    virtual int value() const = 0;
};

struct GetValue {
    int operator()(const PolyNode& n) const { return n.value(); }
};

template<typename Int, Int Value>
struct ConstantNode : public PolyNode {
    virtual int value() const override { return static_cast<int>(Value); }
};

template<typename Int>
struct VariableNode : public PolyNode {
    Int val;
    VariableNode(Int v) : val(v) {}
    virtual int value() const override { return static_cast<int>(val); }
};

int main() {
    bst::wavl<PolyNode, int, GetValue> a;
    VariableNode<char> cn[3] = {'a', 'b', 'c'};
    VariableNode<int> in[4] = {5, 6, 7, 8};
    ConstantNode<int, 3> ci5;
    ConstantNode<short, 9> cs7;
    ConstantNode<unsigned int, 4> cu4;

    for(int i = 0; i < 3; ++i) {
        a.insert(&cn[i]);
    }
    for(int i = 0; i < 4; ++i) {
        a.insert(&in[i]);
    }
    a.insert(&ci5);
    a.insert(&cs7);
    a.insert(&cu4);
    a.erase(a.search(5));

    std::cout << "search 9 result: " << a.search(9) << ", expect: " << &cs7 << std::endl;
    std::cout << "search 1 result: " << a.search(1) << ", expect: " << static_cast<PolyNode*>(nullptr) << std::endl;
    std::cout << "search 5 result: " << a.search(5) << ", expect: " << static_cast<PolyNode*>(nullptr) << std::endl;

    std::cout << "Elements in [4, 10): ";
    for(auto& node : bst::range(a.search_range(4, 10))) {
        std::cout << node.value() << " ";
    }
    std::cout << std::endl;

    std::cout << "All elements in reversed order: ";
    for(auto& node : bst::rrange(a)) { // reversed range
        std::cout << node.value() << " ";
    }
    std::cout << std::endl;
    return 0;
}
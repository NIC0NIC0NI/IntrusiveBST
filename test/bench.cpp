#include<random>
#include<vector>
#include<algorithm>
#include<iostream>
#include<iomanip>
#include<sys/time.h>
#include"bstree.h"

#define TIME_DIFF(start, stop) 1e3 * (stop.tv_sec - start.tv_sec) + 1e-3 * (stop.tv_usec - start.tv_usec)

struct IntNode : public bst::node_hook {
    int val;
};

struct GetValue {
    int operator()(const IntNode& n) const { return n.val; }
};

template<typename BST, typename Nodes, typename Indices>
void test_bst(int size, int n_mod, int n_sch, Nodes& nodes, const Indices& search_idx, const Indices& erase_idx, const char* name) {
    double t11, t12, t13;
    timeval start, stop;

    BST a;

    for(int i = 0; i < size; ++i) {
        a.insert(&nodes[i]);
    }
    for(int i = 0; i < n_mod; ++i) {
        a.erase(&nodes[i]);
    }

    gettimeofday(&start, nullptr);
    for(int i = 0; i < n_mod; ++i) {
        a.insert(&nodes[i]);
    }
    gettimeofday(&stop, nullptr);
    t11 = TIME_DIFF(start, stop);

    gettimeofday(&start, nullptr);
    for(auto& i : search_idx) {
        auto val = nodes[i].val;
        auto p = a.search(val);
        if(p->val != val) {
            std::cout << name << " Wrong" << std::endl;
        }
    }
    gettimeofday(&stop, nullptr);
    t12 = TIME_DIFF(start, stop);

    gettimeofday(&start, nullptr);
    for(auto& i : erase_idx) {
        a.erase(&nodes[i]);
    }
    gettimeofday(&stop, nullptr);
    t13 = TIME_DIFF(start, stop);
    std::cout << "    " << name << ":\t" << t11 << " ms, " << t12 << " ms, " << t13 << " ms" << std::endl;
}

void test_raw(int size, int n_mod, int n_sch, std::uint64_t seed) {
    std::vector<IntNode> nodes (size);

    std::mt19937_64 g(seed);
    std::uniform_int_distribution<int> rd (0, size - 1);
    
    for(std::size_t i = 0; i < size; ++i) {
        nodes[i].val = i;
    }

    std::vector<int> search_idx (n_sch), erase_idx (n_mod);
    for(auto& i : search_idx) {
        i = rd(g);
    }
    for(auto& i : erase_idx) {
        i = rd(g);
    }

    std::sort(erase_idx.begin(), erase_idx.end());
    erase_idx.erase(std::unique(erase_idx.begin(), erase_idx.end()), erase_idx.end());
    
    std::cout << "Testing with intrusive tree: maximum size = " << size << ", #insert = " << n_mod << \
        ", #search = " << n_sch << ", #erase = " << erase_idx.size() << ", random seed = " << seed << std::endl;

    std::cout << "Ordered test:" << std::endl;
    test_bst<bst::rbtree<IntNode, int, GetValue>>(size, n_mod, n_sch, nodes, search_idx, erase_idx, "RB-Tree");
    test_bst<bst::avl<IntNode, int, GetValue>>(size, n_mod, n_sch, nodes, search_idx, erase_idx, "AVL    ");
    test_bst<bst::wavl<IntNode, int, GetValue>>(size, n_mod, n_sch, nodes, search_idx, erase_idx, "WAVL   ");

    std::shuffle(nodes.begin(), nodes.end(), g);
    std::shuffle(erase_idx.begin(), erase_idx.end(), g);
    std::cout << "Random test:" << std::endl;
    test_bst<bst::rbtree<IntNode, int, GetValue>>(size, n_mod, n_sch, nodes, search_idx, erase_idx, "RB-Tree");
    test_bst<bst::avl<IntNode, int, GetValue>>(size, n_mod, n_sch, nodes, search_idx, erase_idx, "AVL    ");
    test_bst<bst::wavl<IntNode, int, GetValue>>(size, n_mod, n_sch, nodes, search_idx, erase_idx, "WAVL   ");
}

int main(int argc, char **argv) {
    int size = 100000;
    int n_mod = 5000;
    int n_sch = 10000;
    int seed = 123241233;

    if(argc > 1) {
        int t = n_mod, s = atoi(argv[1]);
        if(argc > 2) {
            t = atoi(argv[2]);
            if(s >= t && t > 0) {
                size = s, n_mod = t;
            }
        }
    }

    if(argc > 3) {
        int r = atoi(argv[3]);
        if(r > 0) {
            n_sch = r;
        }
    }

    if(argc > 4) {
        seed = atoi(argv[4]);
    }

    test_raw(size, n_mod, n_sch, seed);

    return 0;
}

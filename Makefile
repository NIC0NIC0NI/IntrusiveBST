default:asm/bstree.s bin/bench bin/poly

CXX = g++
CFLAGS = -std=c++11 -O2 -I./include

bin/bench:obj/bench.o obj/bstree.o
	$(CXX) $^ -o $@

bin/poly:obj/poly.o obj/bstree.o
	$(CXX) $^ -o $@

obj/bench.o:test/bench.cpp include/bstree.h
	$(CXX) $(CFLAGS) -c $< -o $@

obj/poly.o:test/poly.cpp include/bstree.h
	$(CXX) $(CFLAGS) -c $< -o $@

obj/bstree.o:src/bstree.cpp include/bstree.h
	$(CXX) $(CFLAGS) -c $< -o $@

asm/bstree.s:src/bstree.cpp include/bstree.h
	$(CXX) $(CFLAGS) -S $< -o $@

clean:
	rm bin/* obj/* asm/*
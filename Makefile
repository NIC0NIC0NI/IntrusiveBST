default:src/bstree.s bench poly

CXX = g++
CFLAGS = -std=c++11 -O2 -I./include

bench:test/bench.o src/bstree.o
	$(CXX) $^ -o $@

poly:test/poly.o src/bstree.o
	$(CXX) $^ -o $@

test/bench.o:test/bench.cpp include/bstree.h
	$(CXX) $(CFLAGS) -c $< -o $@

test/poly.o:test/poly.cpp include/bstree.h
	$(CXX) $(CFLAGS) -c $< -o $@

src/bstree.o:src/bstree.cpp include/bstree.h
	$(CXX) $(CFLAGS) -c $< -o $@

src/bstree.s:src/bstree.cpp include/bstree.h
	$(CXX) $(CFLAGS) -S $< -o $@

clean:
	rm poly bench src/*.o src/*.s test/*.o
# sputil

# TODO
# concurrent
- Dekker
- trieber stack

# type safe uint8_t uint16_t ...
u8
u16
...

#string
utf8
suffix tree https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english/9513423#9513423

#file path
			Path p;
			p.pop(); = cd .. = path.drop_from_right_until("/")
			p.push(s.split(/).foreach); = cd s = . = nop
													.. = p.pop()
													s  = path.push(s)

# Distinct numeric
DISTINCT_NUMERIC(name,underlying_type)

## src/util/Either.h
```cpp
template<typename L, typename R>
struct Either;

Left l(failure);
Right r(value);
```

## sort
- radix sort -https://github.com/yourtion/LearningMasteringAlgorithms-C/blob/master/source/rxsort.c

## Diffie Hellman
## RSA
...

## Blowfish
### bcrypt
### cbc,ecb ...


## timeout
60seconds
1hour


[(0,30),(5,10),(15,20)]:false
[(7,10),(2,4)]: true
- array is a calender day
- tuple consist of start and end time
= Can attend all meetings?

# Graph-coloring
- DSATUR (perfetct colouring?)
- lmXRLF [http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.59.2645&rep=rep1&type=pdf]
- [https://github.com/boostorg/graph/blob/develop/include/boost/graph/sequential_vertex_coloring.hpp]
  [https://www.boost.org/doc/libs/1_57_0/libs/graph/doc/sequential_vertex_coloring.html]
- [https://compscicenter.ru/media/course_class_attachments/Thore_Husfeldt_Graph_colouring_algorithms.pdf]
- [https://www.geeksforgeeks.org/graph-coloring-set-2-greedy-algorithm/]

#
- Markov chains

# compress
- Huffman - https://medium.com/@TravCav/huffman-coding-1e914c419912
- lz77 - https://github.com/yourtion/LearningMasteringAlgorithms-C/blob/master/source/lz77.c

# Consensus
- paxos
- raft

# Heap
- Fibonacci

# filter
- Cuckoofilter

#
xor-swap

# Cellular Automaton
http://mathworld.wolfram.com/CellularAutomaton.html

# hashmap
https://rocksdb.org/blog/2018/08/23/data-block-hash-index.html
https://github.com/stanford-futuredata/index-baselines/blob/master/hashing.cpp
https://reddit.com/comments/9lw5pu "Advanced techniques to implement fast hash tables"

# TODO
- https://www.geeksforgeeks.org/rat-in-a-maze-backtracking-2/ | maze
- https://www.geeksforgeeks.org/longest-palindromic-subsequence-dp-12/
- https://www.michaelfogleman.com/rush/ | rush hour
- https://en.wikipedia.org/wiki/Luhn_algorithm
- knapsack
- traveling salesman problem

## Graph
- https://bradfieldcs.com/algos/graphs/prims-spanning-tree-algorithm/ |prim
- Kruskal
- |Bellman–Ford algorithm
- |Floyd–Warshall_algorithm
- https://www.redblobgames.com/pathfinding/a-star/introduction.html |A* search algorithm
  http://theory.stanford.edu/~amitp/GameProgramming/AStarComparison.html
  https://github.com/boostorg/graph/blob/develop/include/boost/graph/astar_search.hpp

- https://github.com/boostorg/graph/blob/develop/include/boost/graph/isomorphism.hpp

### Mark-and-Sweep
- Mark-and-Sweep  https://www.geeksforgeeks.org/mark-and-sweep-garbage-collection-algorithm/
                  https://en.wikipedia.org/wiki/Tracing_garbage_collection

## Bellman–Ford algorithm
"https://en.wikipedia.org/wiki/Bellman–Ford_algorithm"
Computes shortest paths from a single source vertex to all of the other vertices
in a weighted digraph

## prim Minimum spanning tree
"https://en.wikipedia.org/wiki/Prim%27s_algorithm"
greedy algorithm that finds a minimum spanning tree for a weighted undirected
graph

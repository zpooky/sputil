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
Boyer–Moore string search algorithm
Knuth-Morris-Pratt
Levenshtein distance
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

## compress
Huffman - https://medium.com/@TravCav/huffman-coding-1e914c419912
lz77 - https://github.com/yourtion/LearningMasteringAlgorithms-C/blob/master/source/lz77.c


## timeout
60seconds
1hour


##
Markov chains

Minimum spanning tree

# Graph
## prim
"https://en.wikipedia.org/wiki/Prim%27s_algorithm"
greedy algorithm that finds a minimum spanning tree for a weighted undirected
graph

## Dijkstra
The process that underlies Dijkstra's algorithm is similar to the greedy process
used in Prim's algorithm. Prim's purpose is to find a minimum spanning tree that
connects all nodes in the graph; Dijkstra is concerned with only two nodes.
Prim's does not evaluate the total weight of the path from the starting node,
only the individual edges.


## Bellman–Ford algorithm
"https://en.wikipedia.org/wiki/Bellman–Ford_algorithm"
computes shortest paths from a single source vertex to all of the other vertices
in a weighted digraph

# Consensus
- paxos
- raft

# Heap
- Fibonacci

# HashMap
## ProbingHM
```
static const int TOMB_STONE;

//start from index 
def insert(value):
  index = hash(value) % length;
  store_index = index
  while(items[index]!=NULL && items[index] != &TOMB_STONE){
    ++index;
    if(items[index] == NULL)
      TODO ...
  }
  items[index] =value;

//start from index continue until not NULL, if found remove
def remove(value):
  index = hash(value) % length;
  while(items[index] != value){
    if(items[index] == NULL){
      // Not found
      return;
    }
    ++index;
  }
  if(items[index+1]==NULL) {
    items[index] = NULL;
  } else {
    items[index] = &TOMB_STONE;
  }

//start from index and continue until NULL
def lookup(value):
  index = hash(value) % length;
  do {
    if(items[index] != &TOMB_STONE){
      if(items[index] == value){
        return items[index];
      }
    }
  } while(items[index] != NULL);
  return NULL;


```

sputil_test_src = files([
  'heap/binaryHeapTest.cpp',
  'np/longest_palindromic_substringTest.cpp',
  'np/sudokuTest.cpp',
  'np/knapsackTest.cpp',
  'np/nqueensTest.cpp',
  'np/knights_tourTest.cpp',
  'test/gcstruct.cpp',
  'list/FixedListTest.cpp',
  'list/SkipListTest.cpp',
  'list/LinkedListTest.cpp',
  'util/BitsetTest.cpp',
  'util/QuadsetTest.cpp',
  'util/permutationTest.cpp',
  'util/BloomFilterTest.cpp',
  'util/MaybeTest.cpp',
  'util/FeistlNetTest.cpp',
  'encode/hexTest.cpp',
  'io/fileTest.cpp',
  'stack/StackTest.cpp',
  'tests.cpp',
  'buffer/BytesViewTest.cpp',
  'buffer/CircularBufferTest.cpp',
  'buffer/ThingTest.cpp',
  'buffer/CircularByteBufferTest.cpp',
  'buffer/SinkTest.cpp',
  'sort/mergesortTest.cpp',
  'sort/selectionsortTest.cpp',
  'sort/heapsortTest.cpp',
  'sort/insertionsortTest.cpp',
  'sort/introsortTest.cpp',
  'sort/quicksortTest.cpp',
  'prng/randomTest.cpp',
  'hash/fnvTest.cpp',
  'hash/crcTest.cpp',
  'string/levenshteinTest.cpp',
  'string/string_searchTest.cpp',
  'string/StringUtilTest.cpp',
  'other/tmpl.cpp',
  'other/tmplTest.cpp',
  'problem/list_problemTest.cpp',
  'problem/fibonacciTest.cpp',
  'problem/tower_of_hanoiTest.cpp',
  'problem/miscTest.cpp',
  'problem/bst_problemTest.cpp',
  'problem/string_problemTest.cpp',
  'graph/DijkstraTest.cpp',
  'graph/GraphTest.cpp',
  'graph/Graph2Test.cpp',
  'tree/avlTest.cpp',
  'tree/bst_test.cpp',
  'tree/treeTest.cpp',
  'tree/StaticTreeTest.cpp',
  'tree/red-blackTest.cpp',
  'tree/btreeTest.cpp',
  'tree/avlRecTest.cpp',
  'tree/btree_recTest.cpp',
  'map/ProbingHashMapTest.cpp',
  'map/HashSetProbingTest.cpp',
  'map/HashSetOpenTest.cpp',
  'map/HashSetTreeTest.cpp',
  'map/HashMapProbingTest.cpp',
  'concurrent/ReadWriteLockTest.cpp'
])

sputil_test_deps = []
sputil_test_deps += sputil_dep
# sputil_test_deps += dependency('thread')

gtest_dep = dependency('gtest_main', required: false)
if gtest_dep.found()
  sputil_test_deps += gtest_dep

  sputil_test = executable('sputiltest',
                       sputil_test_src,
                       # include_directories: [],
                       # cpp_args: _compile_options,
                       dependencies: sputil_test_deps
                       )

  test('sputil test', sputil_test)
endif

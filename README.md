# sputil

# TODO
## PRNG
support better seeding
https://github.com/v8/v8/blob/c877c779960dcf58eeca1534e85297062114c310/src/base/utils/random-number-generator.cc#L53-L77
```cpp
// Gather entropy from /dev/urandom if available.
FILE* fp = fopen("/dev/urandom", "rb");
if (fp != nullptr) {
  int64_t seed;
  size_t n = fread(&seed, sizeof(seed), 1, fp);
  fclose(fp);
  if (n == 1) {
    SetSeed(seed);
    return;
  }
}

// We cannot assume that random() or rand() were seeded
// properly, so instead of relying on random() or rand(),
// we just seed our PRNG using timing data as fallback.
// This is weak entropy, but it's sufficient, because
// it is the responsibility of the embedder to install
// an entropy source using v8::V8::SetEntropySource(),
// which provides reasonable entropy, see:
// https://code.google.com/p/v8/issues/detail?id=2905
int64_t seed = Time::NowFromSystemTime().ToInternalValue() << 24;
seed ^= TimeTicks::HighResolutionNow().ToInternalValue() << 16;
seed ^= TimeTicks::Now().ToInternalValue() << 8;
SetSeed(seed);
```
## src/memory/UniformStackAllocator.h
statefull stack allocator
- Array[T]
```cpp
class UniformStackAllocator<T>
alloc():*T -> stack.pop()
dealloc(T*) -> stack.push()
```

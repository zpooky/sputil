# sputil

# TODO
# type safe uint8_t uint16_t ...
u8
u16
...

# Distinct numeric
DISTINCT_NUMERIC(name,underlying_type)

## PRNG
support better seeding
https://github.com/v8/v8/blob/c877c779960dcf58eeca1534e85297062114c310/src/base/utils/random-number-generator.cc#L53-L77
```cpp
Xorshift32 r = seeded<Xorshift32>();

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
## src/util/Either.h
```cpp
template<typename L, typename R>
struct Either;

Left l(failure);
Right r(value);
```

## assert with stacktrace support
http://boostorg.github.io/stacktrace/stacktrace/getting_started.html
https://github.com/boostorg/stacktrace

https://github.com/bombela/backward-cpp

## sort
...

## Diffie Hellman
...

## Blowfish
### bcrypt
### cbc,ecb ...

##
```cpp
StaticArray[size](initializer_list<T,size>)
```

## interval 10s 5m 1h

## Sink
struct BufferedSink {
  Buffer b;
  bool (*sink)(Buffer&);
};

bool write(BufferedSink&,)


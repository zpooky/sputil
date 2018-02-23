# sputil

# TODO
# concurrent
- Dekker
- trieber stack

# type safe uint8_t uint16_t ...
u8
u16
...

# Distinct numeric
DISTINCT_NUMERIC(name,underlying_type)

## src/util/Either.h
```cpp
template<typename L, typename R>
struct Either;

Left l(failure);
Right r(value);
```

## !assert with stacktrace support
http://boostorg.github.io/stacktrace/stacktrace/getting_started.html
https://github.com/boostorg/stacktrace

https://github.com/bombela/backward-cpp
###assert with gdb break
programitcly trigger breakpoint when assert fails

## sort
...

## Diffie Hellman
...

## Blowfish
### bcrypt
### cbc,ecb ...

##
```cpp
impl iniializer list with size as a template arg
StaticArray[size](initializer_list<T,size>)
```

## interval 10s 5m 1h

## Sink
struct BufferedSink {
  Buffer b;
  bool (*sink)(Buffer&);
};

bool write(BufferedSink&,)

## "Sink" for reading supporting autofill the buffer


## CircularByteBuffer
- scattter gatter read & write

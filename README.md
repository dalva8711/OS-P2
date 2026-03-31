# OS Project 2 — Memory Manager

A C++ implementation of a configurable memory manager that simulates dynamic memory allocation using pluggable allocation strategies.

## Overview

The `MemoryManager` class manages a contiguous region of memory divided into fixed-size words. It tracks allocated and free regions (holes) using an internal block list, and delegates placement decisions to a user-supplied allocator function. Two built-in allocator strategies are provided: **best-fit** and **worst-fit**.

## Files

| File | Description |
|---|---|
| `MemoryManager.h` | Class declaration, `Block` struct, and built-in allocator functions (`bestFit`, `worstFit`) |
| `MemoryManager.cpp` | Implementation of all `MemoryManager` member functions |

## API

### Constructor / Destructor

```cpp
MemoryManager();
MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
~MemoryManager(); // calls shutdown()
```

### Lifecycle

```cpp
void initialize(size_t sizeInWords); // allocates the managed memory region
void shutdown();                     // releases all memory and resets state
```

### Allocation

```cpp
void *allocate(size_t sizeInBytes);                          // allocate a block; returns nullptr on failure
void  free(void *address);                                   // free a previously allocated block (coalesces adjacent holes)
void  setAllocator(std::function<int(int, void *)> allocator); // swap the placement strategy at runtime
```

### Introspection

```cpp
void    *getList();         // returns a uint16_t hole list: [count, offset0, len0, offset1, len1, ...]
void    *getMemoryStart();  // returns pointer to the start of the managed region
unsigned getWordSize();     // returns the configured word size in bytes
unsigned getMemoryLimit();  // returns the total size of the managed region in bytes
void    *getBitmap();       // (stub) returns a bitmap of allocated/free words
int      dumpMemoryMap(char *filename); // (stub) writes the memory map to a file
```

### Built-in Allocator Functions

Both functions accept a word count and the hole list returned by `getList()`, and return the byte offset of the chosen hole (or `-1` if no suitable hole exists).

```cpp
int bestFit(int sizeInWords, void *list);  // smallest hole that fits
int worstFit(int sizeInWords, void *list); // largest hole that fits
```

## Hole List Format

`getList()` returns a pointer to a contiguous array of `uint16_t` values:

```
[ numHoles | offset_0 | length_0 | offset_1 | length_1 | ... ]
```

- `numHoles` — number of free holes
- `offset_i` — byte offset of hole `i` from the start of the managed region
- `length_i` — size of hole `i` in bytes

## Building

The project has no external dependencies. Compile with any C++11-compatible compiler:

```bash
g++ -std=c++11 -o memory_manager MemoryManager.cpp
```

## Usage Example

```cpp
#include "MemoryManager.h"

int main() {
    // 4-byte words, best-fit placement
    MemoryManager mm(4, bestFit);

    mm.initialize(256);          // manage 256 words (1024 bytes)

    void *a = mm.allocate(100);  // allocate 100 bytes
    void *b = mm.allocate(200);  // allocate 200 bytes

    mm.free(a);                  // release first block (hole coalesced automatically)

    mm.setAllocator(worstFit);   // swap strategy at runtime
    void *c = mm.allocate(50);

    mm.shutdown();
    return 0;
}
```

## Design Notes

- **Block list** — internal state is maintained as a `std::list<Block>`, where each entry records whether the region is a hole or an allocated block, its byte offset, and its length.
- **Coalescing** — `free()` marks a block as a hole and merges contiguous holes on both sides, reducing fragmentation.
- **Word alignment** — allocation requests are rounded up to the nearest whole word before consulting the allocator.
- **Pluggable allocator** — any function matching `int(int sizeInWords, void *holeList)` can be used as a placement policy.

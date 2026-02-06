# Skip List

A C++ implementation of the **Skip List** data structure: an efficient ordered dynamic list with probabilistic balancing.

This project focuses on correctness, performance considerations, and clean container-style API design inspired by the C++ Standard Library.

## Overview

A **skip list** is a randomized data structure that maintains elements in sorted order and supports efficient search, insertion, and deletion in expected **O(log n)** time.

This implementation aims to provide:

- A reusable generic container
- STL-style interface and iterators
- Efficient memory management through a custom memory pool
- Conditional use of trivial destruction optimisations

## Motivation

This project was developed as a learning exercise in:

- Modern C++ container design
- Memory management and allocator strategies
- Template-based library development
- Probabilistic data strucures

## Building

Requirements:

- C++ 17 compatible compiler
- CMake ≥ 3.16

Build instructions:

```
git clone https://github.com/fefo-phef/skip-list.git
cd skip-list

mkdir build
cd build

cmake ..
cmake --build .
```

Running the example:

```
./skiplist_basic
```

Cleaning the build:

```
rm -rf build
```

Using the library in another CMake project:

```
add_subdirectory(skip-list)
target_link_libraries(my_app skiplist)
```

Then in code:

```
#include <skiplist.h>
```

## References

- Bjarne Stroustrup, *Programming: Principles and Practice using C++*
- Dmitry Soshnikov, *[Writing a Pool Allocator](https://dmitrysoshnikov.com/compilers/writing-a-pool-allocator/)*

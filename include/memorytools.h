#ifndef MEMORYTOOLS_H
#define MEMORYTOOLS_H

#include <cstddef>

// when a chunk is free, next points to the following free chunk in memory
// when a chunk is allocated, this memory is being used by other parts of the code

class PoolAllocator {
    struct BlockHeader {
        BlockHeader* next;
    };

    struct Chunk {
        Chunk* next;
    };

    BlockHeader* block_head_ = nullptr;
    size_t chunk_size_;
    size_t chunks_per_block_;
    Chunk* allocation_ptr_ = nullptr;
    Chunk* AllocateBlock();

    public:
    PoolAllocator(size_t chunk_size, size_t chunks_per_block);

    PoolAllocator(PoolAllocator&&);
    PoolAllocator& operator=(PoolAllocator&&);

    ~PoolAllocator();

    void* Allocate();
    void Deallocate(void* ptr);
};

#endif
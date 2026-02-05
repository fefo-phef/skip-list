#include <cstdlib>
#include <cstdint>
#include "memorytools.h"

PoolAllocator::PoolAllocator(size_t chunk_size, size_t chunks_per_block) :  chunk_size_{chunk_size}, chunks_per_block_{chunks_per_block} {
    if (chunk_size_ < sizeof(Chunk)) {
        chunk_size_ = sizeof(Chunk);
    }
}

PoolAllocator::PoolAllocator(PoolAllocator&& other) : block_head_{other.block_head_}, chunk_size_{other.chunk_size_}, chunks_per_block_{other.chunks_per_block_}, allocation_ptr_{other.allocation_ptr_} {
    other.block_head_ = nullptr;
    other.allocation_ptr_ = nullptr;
}

// we assume that the structure using PoolAllocator will properly call the destructors
// for the non-trivial objects stored before this move assignment is called

PoolAllocator& PoolAllocator::operator=(PoolAllocator&& other) {
    if (this != &other) {
        while (block_head_ != nullptr) {
            BlockHeader* next = block_head_->next;
            delete[] reinterpret_cast<char*>(block_head_);
            block_head_ = next;
        }

        block_head_ = other.block_head_;
        chunk_size_ = other.chunk_size_;
        chunks_per_block_ = other.chunks_per_block_;
        allocation_ptr_ = other.allocation_ptr_;

        other.block_head_ = nullptr;
        other.allocation_ptr_ = nullptr;
    }
    return *this;
}

PoolAllocator::~PoolAllocator() {
    while (block_head_ != nullptr) {
        BlockHeader* next = block_head_->next;
        delete[] reinterpret_cast<char*>(block_head_);
        block_head_ = next;
    }
}

PoolAllocator::Chunk* PoolAllocator::AllocateBlock() {
    size_t block_size = sizeof(BlockHeader) + chunk_size_ * chunks_per_block_ + alignof(std::max_align_t);
    char* start = new char[block_size];

    BlockHeader* block_start = reinterpret_cast<BlockHeader*>(start);
    block_start->next = block_head_;
    block_head_ = block_start;

    uintptr_t to_align = reinterpret_cast<uintptr_t>(start + sizeof(BlockHeader));
    uintptr_t aligned = (to_align + alignof(std::max_align_t) - 1) & ~(alignof(std::max_align_t) - 1);
    Chunk* chunk_start = reinterpret_cast<Chunk*>(aligned);

    Chunk* setup = chunk_start;
    for (int i = 0; i < chunks_per_block_ - 1; ++i) {
        setup->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(setup) + chunk_size_);
        setup = setup->next;
    }
    setup->next = nullptr;

    return chunk_start;
}

void* PoolAllocator::Allocate() {
    if (allocation_ptr_ == nullptr) {allocation_ptr_ = AllocateBlock();}

    Chunk* free_memory = allocation_ptr_;
    allocation_ptr_ = allocation_ptr_->next;
    return free_memory;
}

void PoolAllocator::Deallocate(void* chunk) {
    reinterpret_cast<Chunk*>(chunk)->next = allocation_ptr_;
    allocation_ptr_ = reinterpret_cast<Chunk*>(chunk);
}
//-----------------------------------
// ArenaAllocator.cpp
// Caleb Davis
// Allocator that uses pointer moving for quick and simple memory.
//-------------------------------------

#include "engine/core/memory/ArenaAllocator.h"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#if defined(PLATFORM_WINDOWS)
#include <malloc.h>
#endif

ArenaAllocator::~ArenaAllocator() {
    Shutdown();
}

void ArenaAllocator::Init(size_t sizeBytes) {
    assert(m_memory == nullptr && "ArenaAllocator already initialized");
    // round up to multiple of 64 — required by aligned_alloc on Linux,
    // good practice everywhere for cache line alignment
    size_t alignedSize = (sizeBytes + 63) & ~63;
#if defined(PLATFORM_WINDOWS)
    m_memory = static_cast<uint8_t*>(_aligned_malloc(alignedSize, 64));
#else
    m_memory = static_cast<uint8_t*>(aligned_alloc(64, alignedSize));
#endif    
    assert(m_memory != nullptr && "ArenaAllocator failed to allocate memory");
    m_size = sizeBytes;
    m_offset = 0;
}

void ArenaAllocator::Shutdown() {
    if (m_memory) {
#if defined(PLATFORM_WINDOWS)
        _aligned_free(m_memory);
#else
        free(m_memory);
#endif
        m_memory = nullptr;
    }
    m_size = 0;
    m_offset = 0;
}


void* ArenaAllocator::Alloc(size_t size, size_t alignment) {
    // align the current offset up to the required alignment
    //Bit manipulation at its finest
    size_t aligned = (m_offset + alignment - 1) & ~(alignment - 1);

    // check we have enough space
    // assert(aligned + size <= m_size && "ArenaAllocator out of memory");
    if (aligned + size > m_size) return nullptr;

    void* ptr = m_memory + aligned;
    m_offset = aligned + size;
    return ptr;
}

void ArenaAllocator::Reset() {
#ifdef ENGINE_DEBUG
    memset(m_memory, 0xCD, m_offset);
#endif
    m_offset = 0;
}

bool ArenaAllocator::Contains(const void* ptr) const {
    return ptr >= m_memory && ptr < m_memory + m_size;
}
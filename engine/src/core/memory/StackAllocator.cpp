//-----------------------------------
// StackAllocator.cpp
// Caleb Davis
// Allocator that uses pointer movement. Can pop off the top
//-------------------------------------

#include "engine/core/memory/StackAllocator.h"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#if defined(PLATFORM_WINDOWS)
#include <malloc.h>
#endif


StackAllocator::~StackAllocator() {
    Shutdown();
}


void StackAllocator::Init(size_t sizeBytes) {
    assert(m_memory == nullptr && "StackAllocator already initialized");
    // round up to multiple of 64 — required by aligned_alloc on Linux,
    // good practice everywhere for cache line alignment
    size_t alignedSize = (sizeBytes + 63) & ~63;
#if defined(PLATFORM_WINDOWS)
    m_memory = static_cast<uint8_t*>(_aligned_malloc(alignedSize, 64));
#else
    m_memory = static_cast<uint8_t*>(aligned_alloc(64, alignedSize));
#endif   
    assert(m_memory != nullptr && "StackAllocator failed to allocate memory");
    m_size = sizeBytes;
    m_offset = 0;
}

void StackAllocator::Shutdown() {
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

void* StackAllocator::Alloc(size_t size, size_t alignment) {
    size_t aligned = (m_offset + alignment - 1) & ~(alignment - 1);

    // assert(aligned + size <= m_size && "StackAllocator out of memory");
    if (aligned + size > m_size) return nullptr;

    void* ptr = m_memory + aligned;
    m_offset = aligned + size;
    return ptr;
}

void StackAllocator::FreeToMarker(size_t marker) {
    assert(marker <= m_offset && "Marker is ahead of current offset");

#ifdef ENGINE_DEBUG
    memset(m_memory + marker, 0xCD, m_offset - marker);
#endif

    m_offset = marker;
}


void StackAllocator::Reset() {
#ifdef ENGINE_DEBUG
    memset(m_memory, 0xCD, m_offset);
#endif
    m_offset = 0;
}

bool StackAllocator::Contains(const void* ptr) const {
    return ptr >= m_memory && ptr < m_memory + m_size;
}
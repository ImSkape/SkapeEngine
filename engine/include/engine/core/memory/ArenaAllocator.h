//-----------------------------------
// ArenaAllocator.h
// Caleb Davis
// Allocator that uses pointer moving for quick and simple memory.
//-------------------------------------

#pragma once
#include <cstdint>
#include <cstddef>

class ArenaAllocator {
public:
    ArenaAllocator() = default;
    ~ArenaAllocator();

    // allocate backing memory: call once after construction
    void Init(size_t sizeBytes);
    void Shutdown();

    // allocate from arena: just moves pointer forward
    void* Alloc(size_t size, size_t alignment = 8);

    // typed convenience
    template<typename T>
    T* Alloc(size_t count = 1) {
        return static_cast<T*>(Alloc(sizeof(T) * count, alignof(T)));
    }

    // free everything: pointer resets to start
    void Reset();

    // debug
    size_t GetUsed()     const { return m_offset; }
    size_t GetCapacity() const { return m_size; }

    bool Contains(const void* ptr) const;

private:
    uint8_t* m_memory = nullptr;
    size_t   m_size = 0;
    size_t   m_offset = 0;
};
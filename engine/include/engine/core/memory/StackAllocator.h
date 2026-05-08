//-----------------------------------
// StackAllocator.h
// Caleb Davis
// Allocator that uses pointer movement. Can pop off the top
//-------------------------------------

#pragma once
#include <cstdint>
#include <cstddef>

class StackAllocator {
public:
    StackAllocator() = default;
    ~StackAllocator();

    void Init(size_t sizeBytes);
    void Shutdown();

    // save current top — restore with FreeToMarker
    size_t GetMarker() const { return m_offset; }

    // allocate from top
    void* Alloc(size_t size, size_t alignment = 8);

    template<typename T>
    T* Alloc(size_t count = 1) {
        return static_cast<T*>(Alloc(sizeof(T) * count, alignof(T)));
    }

    // free everything back to a saved marker
    void FreeToMarker(size_t marker);

    // free everything
    void Reset();

    bool Contains(const void* ptr) const;


    size_t GetUsed()     const { return m_offset; }
    size_t GetCapacity() const { return m_size; }

private:
    uint8_t* m_memory = nullptr;
    size_t   m_size = 0;
    size_t   m_offset = 0;
};

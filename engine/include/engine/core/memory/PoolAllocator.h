//-----------------------------------
// PoolAllocator.h
// Caleb Davis
// Allocator that allocates fixed sized chunks
//-------------------------------------

#pragma once
#include <cstdint>
#include <cstddef>

template<typename T>
class PoolAllocator {
public:
    PoolAllocator() = default;
    ~PoolAllocator() { Shutdown(); }

    void Init(size_t count) {
        m_count = count;
        m_slotSize = std::max(sizeof(T), sizeof(FreeNode*));


        m_memory = new uint8_t[m_slotSize * count];

        // build free list - each free slot points to the next
        m_freeList = nullptr;
        for (size_t i = 0; i < count; i++) {
            FreeNode* node = reinterpret_cast<FreeNode*>(m_memory + i * m_slotSize);
            node->next = m_freeList;
            m_freeList = node;
        }
    }

    void Shutdown() {
        delete[] m_memory;
        m_memory = nullptr;
        m_freeList = nullptr;
        m_count = 0;
        m_used = 0;
        m_slotSize = 0;
    }

    T* Alloc() {
        if (!m_freeList) return nullptr;  // pool exhausted
        FreeNode* node = m_freeList;
        m_freeList = node->next;
        m_used++;
        return reinterpret_cast<T*>(node);
    }

    void Free(T* ptr) {
        if (!ptr) return;
        FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
        node->next = m_freeList;
        m_freeList = node;
        m_used--;
    }

    size_t GetUsed()     const { return m_used; }
    size_t GetCapacity() const { return m_count; }

private:
    // free list is stored inside the free slots themselves
    // each free slot reinterprets its memory as a FreeNode
    struct FreeNode {
        FreeNode* next = nullptr;
    };

    uint8_t* m_memory = nullptr;
    FreeNode* m_freeList = nullptr;
    size_t    m_count = 0;
    size_t    m_used = 0;
    size_t m_slotSize = 0;
};


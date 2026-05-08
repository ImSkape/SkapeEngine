//-----------------------------------
// MemorySystem.h
// Caleb Davis
// 
//-------------------------------------

#pragma once
#include "engine/core/memory/ArenaAllocator.h"
#include "engine/core/memory/PoolAllocator.h"
#include "engine/core/memory/StackAllocator.h"
#include "engine/core/SEResult.h"
#include <cstdint>

//class ArenaAllocator;
//class StackAllocator;
//template<typename T>
//class PoolAllocator;
//
//// fixed arena categories — each gets its own pre-sized block
//enum class MemoryCategory : uint8_t {
//    Frame,    // per-frame temp data — reset every frame
//    ECS,      // entity/component chunk storage
//    Asset,    // asset loading scratch memory
//    Render,   // render system temp buffers
//    Audio,    // audio system working memory
//    General   // everything else
//};
//
//class MemorySystem {
//public:
//    SEResult Init();
//    void     Shutdown();
//
//    // get an arena for a specific category
//    ArenaAllocator& GetArena(MemoryCategory category);
//
//    // get a stack allocator for scoped temp memory
//    StackAllocator& GetStack();
//
//    // create a pool — caller owns it, backed by General arena
//    // T = type to pool, count = max simultaneous allocations
//    template<typename T>
//    PoolAllocator<T> CreatePool(uint32_t count);
//
//    // reset per-frame arenas — call once at start of each frame
//    void ResetFrameArenas();
//
//    // debug — how much of each arena is used
//    void PrintStats() const;
//
//private:
//    ArenaAllocator m_arenas[(int)MemoryCategory::General + 1];
//    StackAllocator m_stack;
//
//    bool m_initialized = false;
//};
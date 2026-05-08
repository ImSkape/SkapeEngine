//-----------------------------------
// MemorySystem.h
// Caleb Davis
// Handles each memory allocator
//-------------------------------------

// engine/include/engine/core/memory/MemorySystem.h
#pragma once
#include "engine/core/System.h"
#include "engine/core/memory/ArenaAllocator.h"
#include "engine/core/memory/PoolAllocator.h"
#include "engine/core/memory/StackAllocator.h"
#include <cstdint>
#include <string>

enum class MemoryCategory : uint8_t {
    Frame,
    ECS,
    Asset,
    Render,
    Audio,
    General,
    Count
};


struct MemoryConfig {
    size_t frameSize = 4 * 1024 * 1024;  //  4MB — per frame temp data
    size_t ecsSize = 64 * 1024 * 1024;  // 64MB — entity/component chunks
    size_t assetSize = 32 * 1024 * 1024;  // 32MB — asset loading scratch
    size_t renderSize = 16 * 1024 * 1024;  // 16MB — render temp buffers
    size_t audioSize = 8 * 1024 * 1024;  //  8MB — audio working memory
    size_t generalSize = 32 * 1024 * 1024;  // 32MB — everything else
    size_t stackSize = 8 * 1024 * 1024;  //  8MB — scoped temp memory
};


class MemorySystem : public System {
public:
    SEResult Init() override;
    void     Shutdown() override;


    void Configure(const MemoryConfig& config);

    ArenaAllocator& GetArena(MemoryCategory category);
    StackAllocator& GetStack();

    template<typename T>
    PoolAllocator<T> CreatePool(size_t count) {
        PoolAllocator<T> pool;
        pool.Init(count);
        return pool;
    }

    void ResetFrameArenas();
    void PrintStats() const;

    const std::string& GetName() const override {
        static std::string name = "MemorySystem";
        return name;
    }

private:
    MemoryConfig   m_config;
    ArenaAllocator m_arenas[(size_t)MemoryCategory::Count];
    StackAllocator m_stack;
};
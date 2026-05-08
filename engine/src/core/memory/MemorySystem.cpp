//-----------------------------------
// MemorySystem.cpp
// Caleb Davis
// Handles each memory allocator
//-------------------------------------

#include "engine/core/memory/MemorySystem.h"
#include <cstdio>
#include <cassert>

void MemorySystem::Configure(const MemoryConfig& config) {
    m_config = config;
}


SEResult MemorySystem::Init() {
    m_arenas[(size_t)MemoryCategory::Frame].Init(m_config.frameSize);
    m_arenas[(size_t)MemoryCategory::ECS].Init(m_config.ecsSize);
    m_arenas[(size_t)MemoryCategory::Asset].Init(m_config.assetSize);
    m_arenas[(size_t)MemoryCategory::Render].Init(m_config.renderSize);
    m_arenas[(size_t)MemoryCategory::Audio].Init(m_config.audioSize);
    m_arenas[(size_t)MemoryCategory::General].Init(m_config.generalSize);
    m_stack.Init(m_config.stackSize);

    m_initialized = true;
    printf("[MemorySystem] Initialized - %.1f MB total\n",
        (float)(m_config.frameSize +
            m_config.ecsSize +
            m_config.assetSize +
            m_config.renderSize +
            m_config.audioSize +
            m_config.generalSize +
            m_config.stackSize) / (1024.0f * 1024.0f));

    return SEResult::ok();
}

void MemorySystem::Shutdown() {
    for (size_t i = 0; i < (size_t)MemoryCategory::Count; i++)
        m_arenas[i].Shutdown();
    m_stack.Shutdown();
    m_initialized = false;
    printf("[MemorySystem] Shutdown\n");
}

ArenaAllocator& MemorySystem::GetArena(MemoryCategory category) {
    assert((size_t)category < (size_t)MemoryCategory::Count
        && "Invalid memory category");
    return m_arenas[(size_t)category];
}

StackAllocator& MemorySystem::GetStack() {
    return m_stack;
}


void MemorySystem::ResetFrameArenas() {
    m_arenas[(size_t)MemoryCategory::Frame].Reset();
}

void MemorySystem::PrintStats() const {
    printf("[MemorySystem] Stats:\n");

    const char* names[] = {
        "Frame", "ECS", "Asset", "Render", "Audio", "General"
    };

    for (size_t i = 0; i < (size_t)MemoryCategory::Count; i++) {
        const auto& arena = m_arenas[i];
        printf("  %-8s %6.2f KB / %6.2f KB (%.1f%%)\n",
            names[i],
            arena.GetUsed() / 1024.0f,
            arena.GetCapacity() / 1024.0f,
            arena.GetCapacity() > 0
            ? (float)arena.GetUsed() / arena.GetCapacity() * 100.0f
            : 0.0f);
    }

    printf("  %-8s %6.2f KB / %6.2f KB (%.1f%%)\n",
        "Stack",
        m_stack.GetUsed() / 1024.0f,
        m_stack.GetCapacity() / 1024.0f,
        m_stack.GetCapacity() > 0
        ? (float)m_stack.GetUsed() / m_stack.GetCapacity() * 100.0f
        : 0.0f);
}
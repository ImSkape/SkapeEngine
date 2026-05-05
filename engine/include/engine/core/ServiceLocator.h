//-----------------------------------
// ServiceLocator.h
// Caleb Davis
// Lookup table for services and systems for distribution
//-------------------------------------

#pragma once
#include <unordered_map>
#include <typeindex>
#include <cassert>

class ServiceLocator {
public:
    template<typename T>
    static void Register(T* service) {
        s_services[typeid(T)] = service;
    }

    template<typename T>
    static T* Get() {
        auto it = s_services.find(typeid(T));
        if (it == s_services.end()) return nullptr;
        return static_cast<T*>(it->second);
    }

    template<typename T>
    static bool Has() {
        return s_services.count(typeid(T)) > 0;
    }

    static void Clear() {
        s_services.clear();
    }

private:
    static std::unordered_map<std::type_index, void*> s_services;
};
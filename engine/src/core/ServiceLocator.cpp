//-----------------------------------
// ServiceLocator.cpp
// Caleb Davis
// Lookup table for services and systems for distribution
//-------------------------------------
#include "engine/core/ServiceLocator.h"

std::unordered_map<std::type_index, void*> ServiceLocator::s_services;
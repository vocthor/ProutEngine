#pragma once

#include <string>

#include "scene/transform.hpp"
#include "model.hpp"

// class Material; // Phase 4 — not yet implemented

struct Entity
{
    std::string name;
    Transform transform{};
    Model *model = nullptr;
    // Material *material = nullptr; // unused until Phase 4.1
};
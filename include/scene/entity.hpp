#pragma once

#include <string>

#include "scene/transform.hpp"
#include "model.hpp"

struct Entity
{
    std::string name;
    Transform transform{};
    Model *model = nullptr;
};
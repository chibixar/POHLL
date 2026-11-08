#pragma once
#include "Component.h"
#include <memory>

class ComponentFactory
{
public:
    static std::unique_ptr<Component> create(int category_id);
};

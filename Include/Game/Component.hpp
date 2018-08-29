/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/EntityHandle.hpp"

/*
    Component

    Base class for component types.
*/

namespace Game
{
    // Forward declarations.
    struct EntityHandle;
    class ComponentSystem;

    // Component base class.
    class Component
    {
    protected:
        Component() = default;

    public:
        virtual bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
        {
            return true;
        }
    };
}

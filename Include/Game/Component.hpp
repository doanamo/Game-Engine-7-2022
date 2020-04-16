/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/EntityHandle.hpp"

/*
    Component

    Base class for component types.
*/

namespace Game
{
    struct EntityHandle;
    class ComponentSystem;

    class Component
    {
    protected:
        Component() = default;
        virtual ~Component() = default;

    public:
        virtual bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
        {
            return true;
        }
    };
}

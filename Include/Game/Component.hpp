/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/EntityHandle.hpp"

/*
    Component

    Base class for component types.
*/

namespace Game
{
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

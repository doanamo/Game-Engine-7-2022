/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
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

        // Delete copy constructor and operator.
        // Derived components must define move operator in order to be used
        // with some containers such as std::vector that requires objects to
        // have MoveInsertable and EmplaceConstructible traits.
        Component(const Component& other) = delete;
        Component& operator=(const Component& other) = delete;

    public:
        virtual bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
        {
            return true;
        }
    };
}

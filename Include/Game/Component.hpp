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

    class Component : private NonCopyable
    {
    protected:
        Component() = default;
        virtual ~Component() = default;

        // Derived components must define move operator in order to be used
        // with some containers such as std::vector that requires objects to
        // have MoveInsertable and EmplaceConstructible traits.
        Component(Component&& other) = default;
        Component& operator=(Component&& other) = default;

    public:
        virtual bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
        {
            return true;
        }
    };
}

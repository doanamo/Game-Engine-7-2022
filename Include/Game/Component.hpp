/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Component

    Base class for component types.
*/

namespace Game
{
    // Forward declaration.
    class ComponentSystem;

    // Component base class.
    class Component
    {
    public:
        virtual bool Initialize(ComponentSystem* componentSystem)
        {
            return true;
        }
    };
}

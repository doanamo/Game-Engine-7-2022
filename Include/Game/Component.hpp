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
    // Component base class.
    class Component
    {
    protected:
        Component() = default;

    public:
        virtual ~Component() = default;
    };
}

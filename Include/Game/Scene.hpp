/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Scene
*/

namespace Game
{
    // Scene interface.
    class Scene
    {
    public:
        // Called when the scene is about to enter.
        virtual void OnEnter()
        {
        }

        // Called when the scene is about to exit.
        virtual void OnExit()
        {
        }

        // Called when the scene need to be updated.
        virtual void OnUpdate(float timeDelta)
        {
        }

        // Called when the scene needs to be drawn.
        virtual void OnDraw(float timeAlpha)
        {
        }
    };
}

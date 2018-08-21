/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Scene
*/

namespace Game
{
    // Scene base class.
    class Scene
    {
    protected:
        // Protected constructor.
        Scene() = default;

    public:
        // Virtual destructor.
        virtual ~Scene() = default;

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
        
        // Returns the scene's printable name.
        virtual const char* GetName() const = 0;

        // Checks if scene implements a custom editor.
        virtual bool HasCustomEditor() const
        {
            return false;
        }
    };
}

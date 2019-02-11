/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Input Manager Editor
*/

namespace Editor
{
    // Input manager editor class.
    class InputManagerEditor
    {
    public:
        InputManagerEditor();
        ~InputManagerEditor();

        // Disallow copying.
        InputManagerEditor(const InputManagerEditor& other) = delete;
        InputManagerEditor& operator=(const InputManagerEditor& other) = delete;

        // Move constructor and assignment.
        InputManagerEditor(InputManagerEditor&& other);
        InputManagerEditor& operator=(InputManagerEditor&& other);

        // Initializes input manager editor.
        bool Initialize(Engine::Root* engine);

        // Updates input manager editor.
        void Update(float timeDelta);

    public:
        // Window state.
        bool mainWindowOpen;

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Initialization state.
        bool m_initialized;
    };
}

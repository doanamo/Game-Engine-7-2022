/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include <System/InputDefinitions.hpp>

namespace System
{
    class Window;
}

/*
    Editor Console

    Interface for console window with log output.
*/

namespace Editor
{
    class EditorConsole final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<EditorConsole>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        ~EditorConsole();

        void Display(float timeDelta);
        void Toggle(bool visibility);
        bool IsVisible() const;

        bool OnKeyboardKey(const System::InputEvents::KeyboardKey& event);

    private:
        EditorConsole();

        System::Window* m_window = nullptr;

        bool m_visible = false;
        bool m_autoScroll = true;
        std::string m_inputBuffer;
    };
}

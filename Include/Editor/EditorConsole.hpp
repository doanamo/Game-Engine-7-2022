/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>

namespace System
{
    class Window;
}

/*
    Editor Console
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

    public:
        ~EditorConsole();

        void Update(float timeDelta);
        void Toggle(bool visibility);
        bool IsVisible() const;

    private:
        EditorConsole();

        System::Window* m_window = nullptr;

        bool m_visible = false;
        std::vector<char> m_inputBuffer;
    };
}
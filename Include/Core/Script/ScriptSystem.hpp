/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/System/EngineSystem.hpp"
#include "Core/Script/ScriptState.hpp"

/*
    Script System

    Responsible for storing and managing main script state.
*/

namespace Core
{
    class ScriptSystem final : public EngineSystem
    {
        REFLECTION_ENABLE(ScriptSystem, EngineSystem)

    public:
        ScriptSystem();
        ~ScriptSystem() override;

        ScriptState& GetState()
        {
            ASSERT(m_state);
            return *m_state;
        }

    protected:
        bool OnAttach(const EngineSystemStorage& systemStorage) override;
        void OnEndFrame() override;

    private:
        std::unique_ptr<ScriptState> m_state;
    };
}

REFLECTION_TYPE(Core::ScriptSystem, Core::EngineSystem)

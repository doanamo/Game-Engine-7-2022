/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/System/SystemStorage.hpp>
#include "Game/GameSystem.hpp"

/*
    Game Instance
*/

namespace Game
{
    class GameInstance final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            FailedSystemCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameInstance>, CreateErrors>;
        static CreateResult Create();

    public:
        ~GameInstance();

        void Tick(float timeDelta);

        const GameSystemStorage& GetSystems() const
        {
            return m_gameSystems;
        }

    private:
        GameInstance();

    private:
        GameSystemStorage m_gameSystems;
    };
}

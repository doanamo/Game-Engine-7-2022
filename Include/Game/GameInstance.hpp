/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Game Instance
*/

namespace Game
{
    class GameSystem;

    class GameInstance final : private Common::NonCopyable
    {
    public:
        using GameSystemPtr = std::unique_ptr<GameSystem>;
        using GameSystemList = std::vector<GameSystemPtr>;
        using GameSystemMap = std::unordered_map<Reflection::TypeIdentifier, GameSystem*>;

        enum class AttachSystemErrors
        {
            NullInstance,
            AlreadyExists,
            FailedAttach,
        };

        using AttachSystemResult = Common::Result<void, AttachSystemErrors>;

        enum class CreateErrors
        {
            FailedGameSystemCreation,
            FailedGameSystemAttach,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameInstance>, CreateErrors>;
        static CreateResult Create();

        enum class FinalizationStates
        {
            Pending,
            Failed,
            Done,
        };

    public:
        ~GameInstance();

        AttachSystemResult AttachSystem(std::unique_ptr<GameSystem>&& gameSystem);
        GameSystem* GetSystem(Reflection::TypeIdentifier typeIdentifier) const;

        template<typename GameSystemType>
        GameSystemType* GetSystem();

        bool Finalize();
        void Tick(float timeDelta);

    private:
        GameInstance();

        GameSystemList m_gameSystemList;
        GameSystemMap m_gameSystemMap;

        FinalizationStates m_finalization = FinalizationStates::Pending;
    };

    template<typename GameSystemType>
    GameSystemType* GameInstance::GetSystem()
    {
        return static_cast<GameSystemType*>(
            GetSystem(Reflection::GetIdentifier<GameSystemType>()));
    }
}

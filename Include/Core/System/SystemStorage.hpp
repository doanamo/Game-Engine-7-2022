/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Utility/Delegate.hpp>
#include "Core/System/SystemInterface.hpp"

/*
    System Storage

    Generic storage for instantiating, attaching, storing and processing unique systems.
*/

namespace Core
{
    template<typename SystemBase, typename StorageContext>
    class SystemStorage final
    {
    public:
        static_assert(std::is_base_of<SystemInterface<SystemBase, StorageContext>, SystemBase>::value,
            "Base system class must derive from system interface template type!");

        static_assert(Reflection::IsReflected<SystemBase>(),
            "Base system class should be reflected!");

        using SystemTypes = std::vector<Reflection::TypeIdentifier>;
        using SystemPtr = std::unique_ptr<SystemBase>;
        using SystemList = std::vector<SystemPtr>;
        using SystemMap = std::unordered_map<Reflection::TypeIdentifier, SystemBase*>;
        using ForEachCallback = Event::Delegate<bool(SystemBase&)>;

    public:
        SystemStorage() = default;
        ~SystemStorage();

        StorageContext& GetContext();
        const StorageContext& GetContext() const;

        bool CreateFromTypes(const SystemTypes& systemTypes);
        bool Attach(std::unique_ptr<SystemBase>&& system);
        bool Finalize();

        template<typename SystemType>
        SystemType& Locate() const;
        SystemBase& Locate(Reflection::TypeIdentifier systemType) const;

        template<typename SystemType>
        SystemType* TryLocate() const;
        SystemBase* TryLocate(Reflection::TypeIdentifier systemType) const;

        void ForEach(ForEachCallback callback);
        void ForEachReverse(ForEachCallback callback);

    private:
        SystemList m_systemList;
        SystemMap m_systemMap;
        StorageContext m_context;
        bool m_finalized = false;
    };

    template<typename SystemBase, typename StorageContext>
    SystemStorage<SystemBase, StorageContext>::~SystemStorage()
    {
        // Destroy attached systems in reverse order.
        for(auto it = m_systemList.rbegin(); it != m_systemList.rend(); ++it)
        {
            it->reset();
        }
    }

    template<typename SystemBase, typename StorageContext>
    StorageContext& SystemStorage<SystemBase, StorageContext>::GetContext()
    {
        return m_context;
    }

    template<typename SystemBase, typename StorageContext>
    const StorageContext& SystemStorage<SystemBase, StorageContext>::GetContext() const
    {
        return m_context;
    }

    template<typename SystemBase, typename StorageContext>
    bool SystemStorage<SystemBase, StorageContext>::CreateFromTypes(const SystemTypes& systemTypes)
    {
        // Construct and attach systems created from type identifiers.
        for(auto systemType : systemTypes)
        {
            SystemPtr createdSystem(Reflection::Construct<SystemBase>(systemType));

            if(createdSystem != nullptr)
            {
                if(!Attach(std::move(createdSystem)))
                    return false;
            }
            else
            {
                LOG_ERROR("Failed to create \"{}\" in \"{}\" system storage!",
                    Reflection::GetName(systemType).GetString(),
                    Reflection::GetName<SystemBase>().GetString());
                return false;
            }
        }

        return true;
    }

    template<typename SystemBase, typename StorageContext>
    bool SystemStorage<SystemBase, StorageContext>::Attach(std::unique_ptr<SystemBase>&& system)
    {
        LOG_PROFILE_SCOPE("System storage \"{}\" attachment of \"{}\"", 
            Reflection::GetName<SystemBase>().GetString(),
            Reflection::GetName(system).GetString());

        LOG_INFO("System storage \"{}\" is attaching \"{}\"...",
            Reflection::GetName<SystemBase>().GetString(),
            Reflection::GetName(system).GetString());

        // Check if storage is finalized.
        if(m_finalized)
        {
            ASSERT(!m_finalized,
                "Cannot attach \"{}\" to storage \"{}\" because it is already finalized!",
                Reflection::GetName(system).GetString(),
                Reflection::GetName<SystemBase>().GetString());
            return false;
        }

        // Check if system is valid for attachment.
        if(system == nullptr)
        {
            LOG_WARNING("Attempted to provide null system to \"{}\" system storage!",
                Reflection::GetName<SystemBase>().GetString());
            return false;
        }

        const Reflection::TypeIdentifier systemType = Reflection::GetIdentifier(system);
        if(const auto it = m_systemMap.find(systemType); it != m_systemMap.end())
        {
            LOG_ERROR("Attempted to provide \"{}\" instance that already exists in "
                "\"{}\" system storage!", Reflection::GetName(systemType).GetString(),
                Reflection::GetName<SystemBase>().GetString());
            return false;
        }

        // Perform system attach to storage.
        // Temporarily toggle finalized state so we can locate other systems.
        {
            m_finalized = true;
            SCOPE_GUARD([this]()
            {
                m_finalized = false;
            });

            auto* systemInterface = static_cast<SystemInterface<SystemBase, StorageContext>*>(system.get());
            if(!systemInterface->OnAttach(*this))
            {
                LOG_ERROR("Failed to attach \"{}\" to \"{}\" system storage!",
                    Reflection::GetName(systemType).GetString(),
                    Reflection::GetName<SystemBase>().GetString());
                return false;
            }
        }

        // Emplace attached system in storage.
        SystemBase* attachedSystem = m_systemList.emplace_back(std::move(system)).get();
        auto [it, result] = m_systemMap.emplace(systemType, attachedSystem);
        ASSERT(result, "Failed to emplace entry in \"{}\" system storage!",
            Reflection::GetName<SystemBase>().GetString());

        return true;
    }

    template<typename SystemBase, typename StorageContext>
    bool SystemStorage<SystemBase, StorageContext>::Finalize()
    {
        ASSERT(!m_finalized, "System storage \"{}\" has already been finalized!",
            Reflection::GetName<SystemBase>().GetString());

        if(!m_finalized)
        {
            // Toggle finalized state now so we can locate other systems.
            m_finalized = true;

            // Finalize all attached systems.
            for(auto& system : m_systemList)
            {
                LOG_PROFILE_SCOPE("System storage \"{}\" finalization of \"{}\"",
                    Reflection::GetName<SystemBase>().GetString(),
                    Reflection::GetName(system).GetString());

                LOG_INFO("System storage \"{}\" is finalizing \"{}\"...",
                    Reflection::GetName<SystemBase>().GetString(),
                    Reflection::GetName(system).GetString());

                ASSERT(system);
                if(!system->OnFinalize(*this))
                {
                    LOG_ERROR("System storage \"{}\" failed to finalize \"{}\"!",
                        Reflection::GetName<SystemBase>().GetString(),
                        Reflection::GetName(system).GetString());
                    return m_finalized = false;
                }
            }
        }

        return true;
    }

    template<typename SystemBase, typename StorageContext>
    template<typename SystemType>
    SystemType& SystemStorage<SystemBase, StorageContext>::Locate() const
    {
        return static_cast<SystemType&>(Locate(Reflection::GetIdentifier<SystemType>()));
    }

    template<typename SystemBase, typename StorageContext>
    SystemBase& SystemStorage<SystemBase, StorageContext>::Locate(
        const Reflection::TypeIdentifier systemType) const
    {
        SystemBase* system = TryLocate(systemType);
        ASSERT(system, "Could not find \"{}\" in \"{}\" system storage!",
            Reflection::GetName(systemType).GetString(),
            Reflection::GetName<SystemBase>().GetString());

        return *system;
    }

    template<typename SystemBase, typename StorageContext>
    template<typename SystemType>
    SystemType* SystemStorage<SystemBase, StorageContext>::TryLocate() const
    {
        return static_cast<SystemType*>(TryLocate(Reflection::GetIdentifier<SystemType>()));
    }

    template<typename SystemBase, typename StorageContext>
    SystemBase* SystemStorage<SystemBase, StorageContext>::TryLocate(
        const Reflection::TypeIdentifier systemType) const
    {
        ASSERT(m_finalized, "Cannot locate systems while storage \"{}\" is not finalized!",
            Reflection::GetName<SystemBase>().GetString());

        // Find system by type identifier.
        const auto it = m_systemMap.find(systemType);
        if(it == m_systemMap.end())
        {
            return nullptr;
        }

        return it->second;
    }

    template<typename SystemBase, typename StorageContext>
    void SystemStorage<SystemBase, StorageContext>::ForEach(ForEachCallback callback)
    {
        ASSERT(m_finalized, "Cannot iterate systems while storage \"{}\" is not finalized!",
            Reflection::GetName<SystemBase>().GetString());

        // Run callback for each system in attachment order.
        // Returning false from callback will abort further processing.
        auto it = m_systemList.begin();
        while(it != m_systemList.end())
        {
            ASSERT(*it);
            if(!callback(*it->get()))
                break;

            ++it;
        }
    }

    template<typename SystemBase, typename StorageContext>
    void SystemStorage<SystemBase, StorageContext>::ForEachReverse(ForEachCallback callback)
    {
        ASSERT(m_finalized, "Cannot iterate systems while storage \"{}\" is not finalized!",
            Reflection::GetName<SystemBase>().GetString());

        // Run callback for each system in reverse attachment order.
        // Returning false from callback will abort further processing.
        auto it = m_systemList.rbegin();
        while(it != m_systemList.rend())
        {
            ASSERT(*it);
            if(!callback(*it->get()))
                break;

            ++it;
        }
    }
}

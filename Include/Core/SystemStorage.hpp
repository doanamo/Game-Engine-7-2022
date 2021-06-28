/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/SystemInterface.hpp"

/*
    System Storage

    Generic storage for instantiating, attaching and storing unique systems.
*/

namespace Core
{
    template<typename SystemBase>
    class SystemStorage final
    {
    public:
        static_assert(std::is_base_of<SystemInterface<SystemBase>, SystemBase>::value,
            "Base system class must derive from system interface template type!");

        static_assert(Reflection::IsReflected<SystemBase>(),
            "Base system class should be reflected!");

        using SystemTypes = std::vector<Reflection::TypeIdentifier>;
        using SystemPtr = std::unique_ptr<SystemBase>;
        using SystemList = std::vector<SystemPtr>;
        using SystemMap = std::unordered_map<Reflection::TypeIdentifier, SystemBase*>;

    public:
        SystemStorage() = default;
        ~SystemStorage();

        bool CreateFromTypes(const SystemTypes& systemTypes);
        bool Attach(std::unique_ptr<SystemBase>&& system);

        SystemBase* Locate(Reflection::TypeIdentifier systemType) const;

        template<typename SystemType>
        SystemType* Locate() const;

    private:
        SystemList m_systemList;
        SystemMap m_systemMap;
    };

    template<typename SystemBase>
    SystemStorage<SystemBase>::~SystemStorage()
    {
        // Destroy attached systems in reverse order.
        for(auto it = m_systemList.rbegin(); it != m_systemList.rend(); ++it)
        {
            it->reset();
        }
    }

    template<typename SystemBase>
    bool SystemStorage<SystemBase>::CreateFromTypes(const SystemTypes& systemTypes)
    {
        for(auto systemType : systemTypes)
        {
            SystemPtr createdSystem(Reflection::Construct<SystemBase>(systemType));

            if(createdSystem != nullptr)
            {
                if(!Attach(std::move(createdSystem)))
                {
                    LOG_ERROR("Could not attach \"{}\" to \"{}\" system storage!",
                        Reflection::GetName(systemType).GetString(),
                        Reflection::GetName<SystemBase>().GetString());
                    return false;
                }
            }
            else
            {
                LOG_ERROR("Could not create \"{}\" in \"{}\" system storage!",
                    Reflection::GetName(systemType).GetString(),
                    Reflection::GetName<SystemBase>().GetString());
                return false;
            }
        }

        // Success!
        return true;
    }

    template<typename SystemBase>
    bool SystemStorage<SystemBase>::Attach(std::unique_ptr<SystemBase>&& system)
    {
        LOG_INFO("Attaching \"{}\" to \"{}\" system storage...",
            Reflection::GetName(system).GetString(),
            Reflection::GetName<SystemBase>().GetString());

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

        // Attach system to storage.
        auto* systemInterface = static_cast<SystemInterface<SystemBase>*>(system.get());
        if(!systemInterface->OnAttach(*this))
        {
            LOG_ERROR("Failed to attach \"{}\" to \"{}\" system storage!",
                Reflection::GetName(systemType).GetString(),
                Reflection::GetName<SystemBase>().GetString());
            return false;
        }

        SystemBase* attachedSystem = m_systemList.emplace_back(std::move(system)).get();
        auto [it, result] = m_systemMap.emplace(systemType, attachedSystem);
        ASSERT(result, "Failed to emplace entry in \"{}\" system storage!",
            Reflection::GetName<SystemBase>().GetString());

        // Success!
        return true;
    }

    template<typename SystemBase>
    SystemBase* SystemStorage<SystemBase>::Locate(
        const Reflection::TypeIdentifier systemType) const
    {
        // Find system by type identifier.
        const auto it = m_systemMap.find(systemType);
        if(it == m_systemMap.end())
        {
            ASSERT(false, "Could not find \"{}\" in \"{}\" system storage!",
                Reflection::GetName(systemType).GetString(),
                Reflection::GetName<SystemBase>().GetString());
            return nullptr;
        }

        return it->second;
    }

    template<typename SystemBase>
    template<typename SystemType>
    SystemType* SystemStorage<SystemBase>::Locate() const
    {
        return static_cast<SystemType*>(
            Locate(Reflection::GetIdentifier<SystemType>()));
    }
}

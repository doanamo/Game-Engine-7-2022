/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    System Interface

    Interface class for base system classes to be used with system storage.
*/

namespace Core
{
    template<typename SystemInterface>
    class SystemStorage;

    template<typename SystemBase>
    class SystemInterface : private Common::NonCopyable
    {
    protected:
        friend SystemStorage<SystemBase>;

        virtual bool OnAttach(const SystemStorage<SystemBase>& systemStorage)
        {
            // Return true to indicate that everything is fine and we can continue.
            return true;
        }
        
        virtual bool OnFinalize(const SystemStorage<SystemBase>& systemStorage)
        {
            // Return true to indicate that everything is fine and we can continue.
            return true;
        }
    };
}

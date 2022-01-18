/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    System Interface

    Interface class for base system classes to be used with system storage.
*/

namespace Core
{
    struct SystemStorageContextEmpty
    {
    };

    template<typename SystemBase, typename StorageContext = SystemStorageContextEmpty>
    class SystemStorage;

    template<typename SystemBase, typename StorageContext = SystemStorageContextEmpty>
    class SystemInterface : private Common::NonCopyable
    {
    protected:
        friend SystemStorage<SystemBase, StorageContext>;

        virtual bool OnAttach(const SystemStorage<SystemBase, StorageContext>& systemStorage)
        {
            // Return true to indicate that everything is fine and we can continue.
            return true;
        }
        
        virtual bool OnFinalize(const SystemStorage<SystemBase, StorageContext>& systemStorage)
        {
            // Return true to indicate that everything is fine and we can continue.
            return true;
        }
    };
}

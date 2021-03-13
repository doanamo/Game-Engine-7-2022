/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Handle.hpp>

/*
    Entity Handle

    For use with entity system. Separate header for breaking dependencies.
*/

namespace Game
{
    struct EntityFlags
    {
        enum
        {
            // Entity handle has been allocated but cannot be used.
            Unused = 0,

            // Entity handle exists and can be referenced.
            // Systems may not acknowledge this entity yet and its
            // components may be still in uninitialized state.
            Exists = 1 << 0,

            // Entity handle exists and has been officially created.
            // Important difference is that other systems have been
            // informed about an entity being created, resulting in
            // its components being initialized as well.
            Created = 1 << 1,

            // Entity handle has been scheduled to be destroyed.
            Destroy = 1 << 2,
        };

        using Type = uint8_t;
    };

    struct EntityEntry
    {
        EntityFlags::Type flags = EntityFlags::Unused;
    };

    using EntityHandle = Common::Handle<EntityEntry>;
}

/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Reflection/Reflection.hpp"

namespace Reflection
{
    namespace Generated
    {
        void RegisterExecutable();
    }

    void Initialize()
    {
        Generated::RegisterExecutable();
    }
}

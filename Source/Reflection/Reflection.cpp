/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

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

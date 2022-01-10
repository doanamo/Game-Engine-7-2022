/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Resettable

    Gives class instance an ability to reset itself to its original state. Legacy code that should
    not be used unless you absolutely know what you are doing.
*/

namespace Common
{
    template<typename Type>
    class Resettable
    {
    public:
        void Reset()
        {
            static_assert(std::is_default_constructible<Type>::value,
                "Cannot reconstruct class instance that is not default constructible!");
            static_assert(std::is_base_of<Resettable<Type>, Type>::value,
                "Incorrect use of resettable base class!");
            static_assert(std::is_final<Type>::value,
                "Resettable base class is dangerous when used with non-final classes!");

            static_cast<Type*>(this)->~Type();
            new (static_cast<Type*>(this)) Type();
        }
    };
}

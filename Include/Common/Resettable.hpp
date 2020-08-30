/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Resettable

    Gives ability for class instance to completely reset itself to its original state. 
*/

namespace Common
{
    template<typename Type>
    class Resettable
    {
    public:
        void Reset()
        {
            static_assert(std::is_base_of<Resettable<Type>, Type>::value, "Incorrect use of resettable base class!");
            static_assert(std::is_final<Type>::value, "Resettable base class is dangerous when used with non-final classes!");

            static_cast<Type*>(this)->~Type();
            new (static_cast<Type*>(this)) Type();
        }
    };
}

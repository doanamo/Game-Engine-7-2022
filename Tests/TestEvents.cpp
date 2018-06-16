/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Events/Delegate.hpp>

char Function(const char* c, int i)
{
    return c[i];
}

class BaseClass
{
public:
    virtual char Method(const char* c, int i)
    {
        return c[i - 1];
    }
};

class DerivedClass : public BaseClass
{
public:
    char Method(const char* c, int i) override
    {
        return c[i + 1];
    }
};

bool TestDelegate()
{
    Delegate<char(const char* c, int i)> delegate;

    if(delegate.IsBound())
        return false;

    // Static function binding.
    delegate.Bind<&Function>();

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != 'w')
        return false;

    // Class method binding.
    BaseClass baseClass;

    delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != ' ')
        return false;

    // Virtual method binding.
    DerivedClass derivedClass;

    delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != 'o')
        return false;

    // Lambda functor binding.
    auto functor = [](const char* c, int i) -> char
    {
        return c[i + 2];
    };

    delegate.Bind(&functor);

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != 'r')
        return false;

    // Delegate unbinding.
    delegate.Bind(nullptr);

    if(delegate.IsBound())
        return false;

    return true;
}

int main()
{
    if(!TestDelegate())
        return 1;

    return 0;
}

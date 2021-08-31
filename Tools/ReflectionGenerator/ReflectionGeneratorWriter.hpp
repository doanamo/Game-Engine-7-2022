/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

class ReflectionGeneratorParser;

/*
    Reflection Generator Writer
*/

class ReflectionGeneratorWriter
{
public:
    ReflectionGeneratorWriter();
    ~ReflectionGeneratorWriter();

    bool WriteReflectionBinding(const ReflectionGeneratorParameters& parameters,
        const ReflectionGeneratorParser& parser);

private:

};

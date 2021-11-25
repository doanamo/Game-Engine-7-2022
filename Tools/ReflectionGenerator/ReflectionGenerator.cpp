/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "ReflectionGeneratorTypes.hpp"
#include "ReflectionGeneratorUtility.hpp"
#include "ReflectionGeneratorCache.hpp"
#include "ReflectionGeneratorParser.hpp"
#include "ReflectionGeneratorWriter.hpp"

int main(int argc, const char* argv[])
{
    const ReflectionGeneratorParameters parameters = ParseCommandLineArguments(argc, argv);
    if(!parameters.isValid)
        return -1;

    ReflectionGeneratorCache cache;
    if(!cache.CollectHeaderFiles(parameters))
        return -1;

    ReflectionGeneratorParser parser;
    if(!parser.ParseReflectedTypes(cache.GetHeaderFiles()))
        return -1;

    ReflectionGeneratorWriter writer;
    if(!writer.WriteReflectionBinding(parameters, parser))
        return -1;

    return 0;
}

/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

struct ReflectionGeneratorParameters;

/*
    Reflection Generator Cache
*/

class ReflectionGeneratorCache
{
public:
    ReflectionGeneratorCache();
    ~ReflectionGeneratorCache();

    bool CollectHeaderFiles(const ReflectionGeneratorParameters& parameters);

    const HeaderFileList& GetHeaderFiles() const
    {
        return m_headerFiles;
    }

private:
    HeaderFileList m_headerFiles;
};

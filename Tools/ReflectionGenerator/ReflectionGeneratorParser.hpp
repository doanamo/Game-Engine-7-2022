/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Reflection Generator Parser
*/

class ReflectionGeneratorParser
{
public:
    ReflectionGeneratorParser();
    ~ReflectionGeneratorParser();

    bool ParseReflectedTypes(const HeaderFileList& headerFiles);

    const ReflectedHeaderList& GetReflectedHeaders() const
    {
        return m_reflectedHeaders;
    }

    const ParsedTypeList& GetParsedTypes() const
    {
        return m_parsedTypes;
    }

    const SortedTypeList& GetSortedTypes() const
    {
        return m_sortedTypes;
    }

private:
    ReflectedHeaderList m_reflectedHeaders;
    ParsedTypeList m_parsedTypes;
    SortedTypeList m_sortedTypes;
};

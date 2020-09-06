/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>

namespace System
{
    class FileHandle;
}

/*
    Texture Atlas

    Stores multiple images that can be referenced by name in a single texture.
*/

namespace Graphics
{
    class RenderContext;
    class Texture;
    class TextureView;

    class TextureAtlas final : private Common::NonCopyable
    {
    public:
        struct LoadFromFile
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedFilePathResolve,
            FailedResourceLoading,
            InvalidResourceContents,
        };

        using CreateResult = Common::Result<std::unique_ptr<TextureAtlas>, CreateErrors>;

        static CreateResult Create();
        static CreateResult Create(System::FileHandle& file, const LoadFromFile& params);

        using ConstTexturePtr = std::shared_ptr<const Texture>;
        using RegionMap = std::unordered_map<std::string, glm::ivec4>;

    public:
        ~TextureAtlas();

        bool AddRegion(std::string name, glm::ivec4 pixelCoords);
        TextureView GetRegion(std::string name);

    private:
        TextureAtlas();

    private:
        ConstTexturePtr m_texture;
        RegionMap m_regions;
    };
};

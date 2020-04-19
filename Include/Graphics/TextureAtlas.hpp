/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

namespace System
{
    class FileSystem;
    class ResourceManager;
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

    class TextureAtlas final : private NonCopyable
    {
    public:
        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
            std::string filePath;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceLoading,
            InvalidResourceContent,
        };

        using CreateResult = Result<std::unique_ptr<TextureAtlas>, CreateErrors>;
        static CreateResult Create();
        static CreateResult Create(const LoadFromFile& params);

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

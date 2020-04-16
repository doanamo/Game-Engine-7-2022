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

    class TextureAtlas final : private NonCopyable, public Resettable<TextureAtlas>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedResourceLoading,
            InvalidResourceContent,
        };

        using InitializeResult = Result<void, InitializeErrors>;

        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
            std::string filePath;
        };

        using ConstTexturePtr = std::shared_ptr<const Texture>;
        using RegionMap = std::unordered_map<std::string, glm::ivec4>;

    public:
        TextureAtlas();
        ~TextureAtlas();

        InitializeResult Initialize();
        InitializeResult Initialize(const LoadFromFile& params);

        bool AddRegion(std::string name, glm::ivec4 pixelCoords);
        TextureView GetRegion(std::string name);

    private:
        ConstTexturePtr m_texture;
        RegionMap m_regions;

        bool m_initialized = false;
    };
};

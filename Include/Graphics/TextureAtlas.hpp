/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
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
    // Forward declarations.
    class RenderContext;
    class Texture;
    class TextureView;

    // Texture atlas class.
    class TextureAtlas
    {
    public:
        // Load texture atlas from a file.
        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            std::string filePath;
        };

        // Type declaration.
        using ConstTexturePtr = std::shared_ptr<const Texture>;
        using RegionMap = std::unordered_map<std::string, glm::ivec4>;

    public:
        TextureAtlas();
        ~TextureAtlas();

        TextureAtlas(const TextureAtlas& other) = delete;
        TextureAtlas& operator=(const TextureAtlas& other) = delete;

        TextureAtlas(TextureAtlas&& other);
        TextureAtlas& operator=(TextureAtlas&& other);

        // Initializes the texture atlas.
        bool Initialize();

        // Loads the texture atlas from a file.
        bool Initialize(const LoadFromFile& params);

        // Adds a named region enclosed in the atlas.
        bool AddRegion(std::string name, glm::ivec4 pixelCoords);

        // Returns a texture view of a region enclosed in the atlas.
        TextureView GetRegion(std::string name);

    private:
        // Texture reference.
        ConstTexturePtr m_texture;

        // Texture regions.
        RegionMap m_regions;

        // Initialization state.
        bool m_initialized;
    };
};

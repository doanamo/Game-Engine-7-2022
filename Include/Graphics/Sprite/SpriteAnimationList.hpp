/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <optional>
#include "Graphics/TextureView.hpp"

// Forward declaration.
namespace System
{
    class FileSystem;
    class ResourceManager;
}

/*
    Sprite Animation List
*/

namespace Graphics
{
    // Sprite animation list class.
    class SpriteAnimationList : private NonCopyable
    {
    public:
        // Loads sprite animations from a file.
        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            std::string filePath;
        };

        // Sprite frame structure.
        struct Frame
        {
            Frame();
            Frame(TextureView&& textureView, float duration);

            TextureView textureView;
            float duration;
        };

        // Animation structure.
        struct Animation
        {
            Animation();

            std::vector<Frame> frames;
            float duration;

            const Frame GetFrameByTime(float animationTime) const;
        };

        // List of animations.
        using AnimationList = std::vector<Animation>;
        using AnimationMap = std::unordered_map<std::string, std::size_t>;

    public:
        SpriteAnimationList();

        // Move constructor and operator.
        SpriteAnimationList(SpriteAnimationList&& other);
        SpriteAnimationList& operator=(SpriteAnimationList&& other);

        // Initializes the sprite animation list instance.
        bool Initialize();

        // Loads sprite animations from a file.
        bool Initialize(const LoadFromFile& params);

        // Gets index of an animation.
        std::optional<std::size_t> GetAnimationIndex(std::string animationName) const;

        // Gets an animation by index.
        const Animation* GetAnimationByIndex(std::size_t animationIndex) const;

    private:
        // Animation storage.
        AnimationList m_animationList;
        AnimationMap m_animationMap;

        // Initialization state.
        bool m_initialized;
    };
}

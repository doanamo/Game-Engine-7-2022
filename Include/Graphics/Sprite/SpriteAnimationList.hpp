/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <optional>
#include <vector>
#include <unordered_map>
#include "Graphics/TextureView.hpp"

namespace System
{
    class FileSystem;
    class ResourceManager;
}

/*
    Animation List
*/

namespace Graphics
{
    class RenderContext;

    class SpriteAnimationList : private NonCopyable
    {
    public:
        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
            std::string filePath;
        };

        struct Frame
        {
            Frame() = default;
            Frame(TextureView&& textureView, float duration);

            TextureView textureView;
            float duration = 0.0f;
        };

        struct Animation
        {
            std::vector<Frame> frames;
            float duration = 0.0f;

            const Frame GetFrameByTime(float animationTime) const;
        };

        using AnimationList = std::vector<Animation>;
        using AnimationMap = std::unordered_map<std::string, std::size_t>;

    public:
        SpriteAnimationList() = default;

        SpriteAnimationList(SpriteAnimationList&& other);
        SpriteAnimationList& operator=(SpriteAnimationList&& other);

        bool Initialize();
        bool Initialize(const LoadFromFile& params);

        std::optional<std::size_t> GetAnimationIndex(std::string animationName) const;
        const Animation* GetAnimationByIndex(std::size_t animationIndex) const;

    private:
        AnimationList m_animationList;
        AnimationMap m_animationMap;
        bool m_initialized = false;
    };
}

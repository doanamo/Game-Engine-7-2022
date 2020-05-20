/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <vector>
#include <optional>
#include <unordered_map>
#include <Core/ServiceStorage.hpp>
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

    class SpriteAnimationList final : private Common::NonCopyable
    {
    public:
        struct LoadFromFile
        {
            const Core::ServiceStorage* services = nullptr;
            std::string filePath;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceLoading,
            InvalidResourceContent,
        };

        using CreateResult = Common::Result<std::unique_ptr<SpriteAnimationList>, CreateErrors>;

        static CreateResult Create();
        static CreateResult Create(const LoadFromFile& params);

        struct Frame
        {
            Frame();
            Frame(TextureView&& textureView, float duration);

            TextureView textureView;
            float duration = 0.0f;
        };

        struct Animation
        {
            std::vector<Frame> frames;
            float duration = 0.0f;

            Frame GetFrameByTime(float animationTime) const;
        };

        using AnimationList = std::vector<Animation>;
        using AnimationMap = std::unordered_map<std::string, std::size_t>;

    public:
        ~SpriteAnimationList();

        std::optional<std::size_t> GetAnimationIndex(std::string animationName) const;
        const Animation* GetAnimationByIndex(std::size_t animationIndex) const;

    private:
        SpriteAnimationList();

    private:
        AnimationList m_animationList;
        AnimationMap m_animationMap;
    };
}

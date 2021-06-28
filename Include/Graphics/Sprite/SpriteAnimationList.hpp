/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include "Graphics/TextureView.hpp"

namespace System
{
    class FileHandle;
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
            const Core::EngineSystemStorage* engineSystems = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceLoading,
            InvalidResourceContents,
        };

        using CreateResult = Common::Result<std::unique_ptr<SpriteAnimationList>, CreateErrors>;

        static CreateResult Create();
        static CreateResult Create(System::FileHandle& file, const LoadFromFile& params);

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
        using AnimationMap = std::unordered_map<std::string, std::uint32_t>;

        using AnimationIndexResult = Common::Result<uint32_t, void>;

        ~SpriteAnimationList();

        AnimationIndexResult GetAnimationIndex(std::string animationName) const;
        const Animation* GetAnimationByIndex(std::size_t animationIndex) const;

    private:
        SpriteAnimationList();

        AnimationList m_animationList;
        AnimationMap m_animationMap;
    };
}

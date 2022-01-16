/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Sprite/SpriteAnimationList.hpp"
#include "Graphics/TextureAtlas.hpp"
#include <Core/System/SystemStorage.hpp>
#include <Core/Resource/ResourceManager.hpp>
using namespace Graphics;

namespace
{
    const char* LogCreateFailed = "Failed to create sprite animation list from \"{}\" file! {}";
}

SpriteAnimationList::Frame::Frame() = default;
SpriteAnimationList::Frame::Frame(TextureView&& textureView, float duration)
    : textureView(std::move(textureView))
    , duration(duration)
{
}

SpriteAnimationList::Frame SpriteAnimationList::Animation::GetFrameByTime(float animationTime) const
{
    // Calculate current frame.
    for(const auto& frame : frames)
    {
        if(animationTime <= frame.duration)
            return frame;

        animationTime -= frame.duration;
    }

    // Return empty frame if animation time does not correspond to any.
    return SpriteAnimationList::Frame();
}

SpriteAnimationList::SpriteAnimationList() = default;
SpriteAnimationList::~SpriteAnimationList() = default;

SpriteAnimationList::CreateResult SpriteAnimationList::Create()
{
    LOG_PROFILE_SCOPE_FUNC();
    auto instance = std::unique_ptr<SpriteAnimationList>(new SpriteAnimationList());
    return Common::Success(std::move(instance));
}

SpriteAnimationList::CreateResult SpriteAnimationList::Create(
    Platform::FileHandle& file, const LoadFromFile& params)
{
    LOG_PROFILE_SCOPE_FUNC();
    LOG("Loading sprite animation list from \"{}\" file...", file.GetPathString());

    CHECK_OR_RETURN(params.engineSystems, Common::Failure(CreateErrors::InvalidArgument));

    // Create base instance.
    auto createResult = Create();
    if(!createResult)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(), "Could not create base instance!");
        return createResult;
    }

    auto instance = createResult.Unwrap();

    // Load resource data.
    std::string jsonString = file.ReadAsTextString();
    jsonString.reserve(jsonString.size() + simdjson::SIMDJSON_PADDING);

    simdjson::ondemand::document json;
    simdjson::ondemand::parser jsonParser;
    if(jsonParser.iterate(jsonString).get(json) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(), "Could not parse file.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    simdjson::ondemand::object spriteAnimationList;
    if(json.find_field("SpriteAnimationList").get(spriteAnimationList) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(),
            "Object \"SpriteAnimationList\" is missing.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    // Load texture atlas.
    std::string_view textureAtlasPath;
    if(spriteAnimationList.find_field("TextureAtlas").get(textureAtlasPath) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(),
            "String \"SpriteAnimationList.TextureAtlas\" is missing.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    TextureAtlas::LoadFromFile textureAtlasParams;
    textureAtlasParams.engineSystems = params.engineSystems;

    auto& resourceManager = params.engineSystems->Locate<Core::ResourceManager>();
    std::shared_ptr<TextureAtlas> textureAtlas = resourceManager.AcquireRelative<TextureAtlas>(
        textureAtlasPath, file.GetPath(), textureAtlasParams)
        .UnwrapOr(nullptr);

    if(textureAtlas == nullptr)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(),
            "Could not load referenced texture atlas!");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    // Read animation entries.
    simdjson::ondemand::array animations;
    if(spriteAnimationList.find_field("Animations").get(animations) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(),
            "Array \"SpriteAnimationList.Animations\" is missing.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    uint32_t animationIndex = 0;
    for(auto animation : animations)
    {
        std::string_view animationName;
        if(animation.find_field("Name").get(animationName) != simdjson::SUCCESS)
        {
            LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                "String \"SpriteAnimationList.Animations[{}].Name\" is missing.",
                animationIndex));
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }

        simdjson::ondemand::array animationFrames;
        if(animation.find_field("Frames").get(animationFrames) != simdjson::SUCCESS)
        {
            LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                "Array \"SpriteAnimationList.Animations[{}:{}].Frames\" is missing.",
                animationIndex, animationName));
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }

        Animation newAnimation;
        uint32_t frameIndex = 0;
        for(auto frame : animationFrames)
        {
            std::string_view frameName;
            if(frame.find_field("Name").get(frameName) != simdjson::SUCCESS)
            {
                LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                    "String \"SpriteAnimationList.Animations[{}:{}].Frames[{}].Name\" is missing.",
                    animationIndex, animationName, frameIndex));
                return Common::Failure(CreateErrors::FailedResourceLoading);
            }

            simdjson::ondemand::number frameDuration;
            if(frame.find_field("Duration").get(frameDuration) != simdjson::SUCCESS)
            {
                LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                    "Number \"SpriteAnimationList.Animations[{}:{}].Frames[{}:{}].Duration\" is missing.",
                    animationIndex, animationName, frameIndex, frameName));
                return Common::Failure(CreateErrors::FailedResourceLoading);
            }

            // Add frame to animation.
            TextureView textureView = textureAtlas->GetRegion(std::string(frameName));
            float duration = static_cast<float>(frameDuration.as_double());

            newAnimation.frames.emplace_back(std::move(textureView), duration);
            newAnimation.duration += duration;

            ++frameIndex;
        }

        // Add animation to list.
        instance->m_animationList.emplace_back(std::move(newAnimation));
        instance->m_animationMap.emplace(animationName, Common::NumericalCast<uint32_t>(
            instance->m_animationList.size() - 1));

        ++animationIndex;
    }

    return Common::Success(std::move(instance));
}

SpriteAnimationList::AnimationIndexResult
    SpriteAnimationList::GetAnimationIndex(std::string animationName) const
{
    auto it = m_animationMap.find(animationName);
    if(it == m_animationMap.end())
    {
        return Common::Failure();
    }

    return Common::Success(it->second);
}

const SpriteAnimationList::Animation*
    SpriteAnimationList::GetAnimationByIndex(std::size_t animationIndex) const
{
    // Make sure that index is valid.
    bool isIndexValid = animationIndex >= 0 && animationIndex < m_animationList.size();
    ASSERT(isIndexValid, "Invalid sprite animation index!");
 
    // Return animation pointed by index.
    return isIndexValid ? &m_animationList[animationIndex] : nullptr;
}

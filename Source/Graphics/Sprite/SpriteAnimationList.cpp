/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Sprite/SpriteAnimationList.hpp"
#include "Graphics/TextureAtlas.hpp"
#include <System/ResourceManager.hpp>
#include <Script/ScriptState.hpp>
using namespace Graphics;

SpriteAnimationList::Frame::Frame() = default;

SpriteAnimationList::Frame::Frame(TextureView&& textureView, float duration) :
    textureView(std::move(textureView)), duration(duration)
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
    LOG("Creating sprite animation list...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<SpriteAnimationList>(new SpriteAnimationList());

    // Success!
    return Common::Success(std::move(instance));
}

SpriteAnimationList::CreateResult SpriteAnimationList::Create(System::FileHandle& file, const LoadFromFile& params)
{
    LOG("Loading sprite animation list from \"{}\" file...", file.GetPath().generic_string());
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire engine services.
    System::ResourceManager* resourceManager = params.services->GetResourceManager();

    // Create base instance.
    auto createResult = Create();
    if(!createResult)
    {
        LOG_ERROR("Could not create base instance!");
        return createResult;
    }

    auto instance = createResult.Unwrap();

    // Load resource script.
    Script::ScriptState::LoadFromFile resourceParams;
    resourceParams.services = params.services;

    auto resourceScript = Script::ScriptState::Create(file, resourceParams).UnwrapOr(nullptr);
    if(resourceScript == nullptr)
    {
        LOG_ERROR("Could not load sprite animation list resource file!");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    // Get global table.
    lua_getglobal(*resourceScript, "SpriteAnimationList");
    SCOPE_GUARD(lua_pop(*resourceScript, 1));

    if(!lua_istable(*resourceScript, -1))
    {
        LOG_ERROR("Table \"SpriteAnimationList\" is missing!");
        return Common::Failure(CreateErrors::InvalidResourceContents);
    }

    // Load texture atlas.
    std::shared_ptr<TextureAtlas> textureAtlas;

    {
        lua_getfield(*resourceScript, -1, "TextureAtlas");
        SCOPE_GUARD(lua_pop(*resourceScript, 1));

        if(!lua_isstring(*resourceScript, -1))
        {
            LOG_ERROR("String \"SpriteAnimationList.TextureAtlas\" is missing!");
            return Common::Failure(CreateErrors::InvalidResourceContents);
        }

        std::filesystem::path textureAtlasPath = lua_tostring(*resourceScript, -1);

        TextureAtlas::LoadFromFile textureAtlasParams;
        textureAtlasParams.services = params.services;

        textureAtlas = resourceManager->AcquireRelative<TextureAtlas>(
            textureAtlasPath, file.GetPath(), textureAtlasParams).UnwrapOr(nullptr);

        if(textureAtlas == nullptr)
        {
            LOG_ERROR("Could not load referenced texture atlas!");
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }
    }

    // Read animation entries.
    lua_getfield(*resourceScript, -1, "Animations");
    SCOPE_GUARD(lua_pop(*resourceScript, 1));

    if(!lua_istable(*resourceScript, -1))
    {
        LOG_ERROR("Table \"SpriteAnimationList.Animations\" is missing!");
        return Common::Failure(CreateErrors::InvalidResourceContents);
    }

    for(lua_pushnil(*resourceScript); lua_next(*resourceScript, -2); lua_pop(*resourceScript, 1))
    {
        // Check if key is a string.
        if(!lua_isstring(*resourceScript, -2))
        {
            LOG_WARNING("Key \"SpriteAnimationList.Animations\" is not a string!");
            LOG_WARNING("Skipping one ill formated sprite animation!");
            continue;
        }

        std::string animationName = lua_tostring(*resourceScript, -2);

        // Read animation frames.
        Animation animation;

        for(lua_pushnil(*resourceScript); lua_next(*resourceScript, -2); lua_pop(*resourceScript, 1))
        {
            // Make sure that we have a table.
            if(!lua_istable(*resourceScript, -1))
            {
                LOG_WARNING("Value in \"SpriteAnimationList.Animations[\"{}\"]\" is not a table!", animationName);
                LOG_WARNING("Skipping one ill formated sprite animation frame!");
                continue;
            }

            // Get sequence frame.
            TextureView textureView;

            {
                lua_pushinteger(*resourceScript, 1);
                lua_gettable(*resourceScript, -2);
                SCOPE_GUARD(lua_pop(*resourceScript, 1));

                if(!lua_isstring(*resourceScript, -1))
                {
                    LOG_WARNING("Field in \"SpriteAnimationList.Animations[{}][0]\" is not a string!", animationName);
                    LOG_WARNING("Skipping one ill formated sprite animation frame!");
                    continue;
                }

                textureView = textureAtlas->GetRegion(lua_tostring(*resourceScript, -1));
            }

            // Get frame duration.
            float frameDuration = 0.0f;

            {
                lua_pushinteger(*resourceScript, 2);
                lua_gettable(*resourceScript, -2);
                SCOPE_GUARD(lua_pop(*resourceScript, 1));

                if(!lua_isnumber(*resourceScript, -1))
                {
                    LOG_WARNING("Field in \"SpriteAnimationList.Animations[\"{}\"][1]\" is not a number!", animationName);
                    LOG_WARNING("Skipping one ill formated sprite animation frame!");
                    continue;
                }

                frameDuration = (float)lua_tonumber(*resourceScript, -1);
            }

            // Add frame to animation.
            animation.frames.emplace_back(std::move(textureView), frameDuration);
            animation.duration += frameDuration;
        }

        // Add animation to list.
        instance->m_animationList.emplace_back(std::move(animation));
        instance->m_animationMap.emplace(animationName,
            Common::NumericalCast<uint32_t>(instance->m_animationList.size() - 1));
    }

    // Success!
    return Common::Success(std::move(instance));
}

SpriteAnimationList::AnimationIndexResult SpriteAnimationList::GetAnimationIndex(std::string animationName) const
{
    auto it = m_animationMap.find(animationName);
    if(it == m_animationMap.end())
    {
        return Common::Failure();
    }

    return Common::Success(it->second);
}

const SpriteAnimationList::Animation* SpriteAnimationList::GetAnimationByIndex(std::size_t animationIndex) const
{
    // Make sure that index is valid.
    bool isIndexValid = animationIndex >= 0 && animationIndex < m_animationList.size();
    ASSERT(isIndexValid, "Invalid sprite animation index!");
 
    // Return animation pointed by index.
    return isIndexValid ? &m_animationList[animationIndex] : nullptr;
}

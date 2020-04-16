/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/Sprite/SpriteAnimationList.hpp"
#include "Graphics/TextureAtlas.hpp"
#include <System/FileSystem.hpp>
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

SpriteAnimationList::InitializeResult SpriteAnimationList::Initialize()
{
    LOG("Initializing sprite animation list...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Success!
    m_initialized = true;
    return Success();
}

SpriteAnimationList::InitializeResult SpriteAnimationList::Initialize(const LoadFromFile& params)
{
    LOG("Loading sprite animation list from \"{}\" file...", params.filePath);
    LOG_SCOPED_INDENT();

    // Initialize sprite animation list instance.
    SUCCESS_OR_RETURN_RESULT(this->Initialize());

    // Setup initialization guard.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, this->Reset());

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.resourceManager != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Failure(InitializeErrors::InvalidArgument));

    // Create script state.
    Script::ScriptState::LoadFromFile scriptParams;
    scriptParams.fileSystem = params.fileSystem;
    scriptParams.filePath = params.filePath;

    Script::ScriptState scriptState;
    if(!scriptState.Initialize(scriptParams))
    {
        LOG_ERROR("Could not load sprite animation list resource file!");
        return Failure(InitializeErrors::FailedResourceLoading);
    }

    // Get global table.
    lua_getglobal(scriptState, "SpriteAnimationList");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"SpriteAnimationList\" is missing!");
        return Failure(InitializeErrors::InvalidResourceContent);
    }

    // Load texture atlas.
    std::shared_ptr<TextureAtlas> textureAtlas;

    {
        lua_getfield(scriptState, -1, "TextureAtlas");
        SCOPE_GUARD(lua_pop(scriptState, 1));

        if(!lua_isstring(scriptState, -1))
        {
            LOG_ERROR("String \"SpriteAnimationList.TextureAtlas\" is missing!");
            return Failure(InitializeErrors::InvalidResourceContent);
        }

        TextureAtlas::LoadFromFile textureAtlasParams;
        textureAtlasParams.fileSystem = params.fileSystem;
        textureAtlasParams.resourceManager = params.resourceManager;
        textureAtlasParams.renderContext = params.renderContext;
        textureAtlasParams.filePath = lua_tostring(scriptState, -1);

        textureAtlas = params.resourceManager->Acquire<TextureAtlas>(textureAtlasParams.filePath, textureAtlasParams);

        if(!textureAtlas)
        {
            LOG_ERROR("Could not load referenced texture atlas!");
            return Failure(InitializeErrors::FailedResourceLoading);
        }
    }

    // Read animation entries.
    lua_getfield(scriptState, -1, "Animations");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"SpriteAnimationList.Animations\" is missing!");
        return Failure(InitializeErrors::InvalidResourceContent);
    }

    for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
    {
        // Check if key is a string.
        if(!lua_isstring(scriptState, -2))
        {
            LOG_WARNING("Key \"SpriteAnimationList.Animations\" is not a string!");
            LOG_WARNING("Skipping one ill formated sprite animation!");
            continue;
        }

        std::string animationName = lua_tostring(scriptState, -2);

        // Read animation frames.
        Animation animation;

        for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
        {
            // Make sure that we have a table.
            if(!lua_istable(scriptState, -1))
            {
                LOG_WARNING("Value in \"SpriteAnimationList.Animations[\"{}\"]\" is not a table!", animationName);
                LOG_WARNING("Skipping one ill formated sprite animation frame!");
                continue;
            }

            // Get sequence frame.
            TextureView textureView;

            {
                lua_pushinteger(scriptState, 1);
                lua_gettable(scriptState, -2);
                SCOPE_GUARD(lua_pop(scriptState, 1));

                if(!lua_isstring(scriptState, -1))
                {
                    LOG_WARNING("Field in \"SpriteAnimationList.Animations[{}][0]\" is not a string!", animationName);
                    LOG_WARNING("Skipping one ill formated sprite animation frame!");
                    continue;
                }

                textureView = textureAtlas->GetRegion(lua_tostring(scriptState, -1));
            }

            // Get frame duration.
            float frameDuration = 0.0f;

            {
                lua_pushinteger(scriptState, 2);
                lua_gettable(scriptState, -2);
                SCOPE_GUARD(lua_pop(scriptState, 1));

                if(!lua_isnumber(scriptState, -1))
                {
                    LOG_WARNING("Field in \"SpriteAnimationList.Animations[\"{}\"][1]\" is not a number!", animationName);
                    LOG_WARNING("Skipping one ill formated sprite animation frame!");
                    continue;
                }

                frameDuration = (float)lua_tonumber(scriptState, -1);
            }

            // Add a frame to the animation.
            animation.frames.emplace_back(std::move(textureView), frameDuration);
            animation.duration += frameDuration;
        }

        // Add an animation to the list.
        m_animationList.emplace_back(std::move(animation));
        m_animationMap.emplace(animationName, m_animationList.size() - 1);
    }

    // Success!
    initialized = true;
    return Success();
}

std::optional<std::size_t> SpriteAnimationList::GetAnimationIndex(std::string animationName) const
{
    // Find animation index by name.
    auto it = m_animationMap.find(animationName);

    if(it != m_animationMap.end())
    {
        return it->second;
    }
    else
    {
        return std::nullopt;
    }
}

const SpriteAnimationList::Animation* SpriteAnimationList::GetAnimationByIndex(std::size_t animationIndex) const
{
    // Make sure that index is valid.
    bool isIndexValid = animationIndex >= 0 && animationIndex < m_animationList.size();
    ASSERT(isIndexValid, "Invalid sprite animation index!");
 
    // Return animation pointed by index.
    return isIndexValid ? &m_animationList[animationIndex] : nullptr;
}

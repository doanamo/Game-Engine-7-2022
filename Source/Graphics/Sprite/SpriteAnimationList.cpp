/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Sprite/SpriteAnimationList.hpp"
#include "Graphics/TextureAtlas.hpp"
#include "Scripting/ScriptState.hpp"
#include "System/ResourceManager.hpp"
#include "Engine/Root.hpp"
using namespace Graphics;

SpriteAnimationList::LoadFromFile::LoadFromFile() :
    engine(nullptr), filePath()
{
}

SpriteAnimationList::Frame::Frame() :
    textureView(), duration(0.0f)
{
}

SpriteAnimationList::Frame::Frame(TextureView&& textureView, float duration) :
    textureView(std::move(textureView)), duration(duration)
{
}

SpriteAnimationList::Animation::Animation() :
    duration(0.0f)
{
}

const SpriteAnimationList::Frame SpriteAnimationList::Animation::GetFrameByTime(float animationTime) const
{
    // Calculate the current frame.
    for(auto& frame : frames)
    {
        if(animationTime <= frame.duration)
            return frame;

        animationTime -= frame.duration;
    }

    // Return empty frame if animation time does not correspond to any.
    return SpriteAnimationList::Frame();
}

SpriteAnimationList::SpriteAnimationList() :
    m_initialized(false)
{
}

SpriteAnimationList::SpriteAnimationList(SpriteAnimationList&& other) :
    SpriteAnimationList()
{
    *this = std::move(other);
}

SpriteAnimationList& SpriteAnimationList::operator=(SpriteAnimationList&& other)
{
    std::swap(m_animationList, other.m_animationList);
    std::swap(m_animationMap, other.m_animationMap);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SpriteAnimationList::Initialize()
{
    LOG("Initializing sprite animation list...");
    LOG_SCOPED_INDENT();

    // Make sure that this instance has not been already initialized.
    VERIFY(!m_initialized, "Sprite animation list has already been initialized!");

    // Create a scope guard in case initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = SpriteAnimationList());

    // Success!
    return m_initialized = true;
}

bool SpriteAnimationList::Initialize(const LoadFromFile& params)
{
    LOG("Loading sprite animation list from \"{}\" file...", params.filePath);
    LOG_SCOPED_INDENT();

    // Initialize the sprite animation list instance.
    if(!this->Initialize())
        return false;

    // Create a scope guard in case initialization fails.
    bool initialized = false;

    SCOPE_GUARD_IF(!initialized, *this = SpriteAnimationList());

    // Validate arguments.
    if(params.engine == nullptr)
    {
        LOG_ERROR("Invalid parameter - \"engine\" is null!");
        return false;
    }

    // Create a script state.
    Scripting::ScriptState::LoadFromFile scriptParams;
    scriptParams.engine = params.engine;
    scriptParams.filePath = params.filePath;

    Scripting::ScriptState scriptState;
    if(!scriptState.Initialize(scriptParams))
    {
        LOG_ERROR("Could not load file!");
        return false;
    }

    // Get the global table.
    lua_getglobal(scriptState, "SpriteAnimationList");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"SpriteAnimationList\" is missing!");
        return false;
    }

    // Load the texture atlas.
    std::shared_ptr<TextureAtlas> textureAtlas;

    {
        lua_getfield(scriptState, -1, "TextureAtlas");
        SCOPE_GUARD(lua_pop(scriptState, 1));

        if(!lua_isstring(scriptState, -1))
        {
            LOG_ERROR("String \"SpriteAnimationList.TextureAtlas\" is missing!");
            return false;
        }

        TextureAtlas::LoadFromFile textureAtlasParams;
        textureAtlasParams.engine = params.engine;
        textureAtlasParams.filePath = lua_tostring(scriptState, -1);

        textureAtlas = params.engine->GetResourceManager().Acquire<TextureAtlas>(
            textureAtlasParams.filePath, textureAtlasParams);

        if(!textureAtlas)
        {
            LOG_ERROR("Could not load texture atlas!");
            return false;
        }
    }

    // Read animation entries.
    lua_getfield(scriptState, -1, "Animations");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"SpriteAnimationList.Animations\" is missing!");
        return false;
    }

    for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
    {
        // Check if the key is a string.
        if(!lua_isstring(scriptState, -2))
        {
            LOG_WARNING("Key in \"SpriteAnimationList.Animations\" is not a string!");
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
    return initialized = true;
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
 
    // Return an animation pointed by the index.
    return isIndexValid ? &m_animationList[animationIndex] : nullptr;
}

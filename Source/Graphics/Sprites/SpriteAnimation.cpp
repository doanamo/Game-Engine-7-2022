/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Sprites/SpriteAnimation.hpp"
#include "Graphics/TextureAtlas.hpp"
#include "Scripts/ScriptState.hpp"
#include "System/ResourceManager.hpp"
#include "Engine.hpp"
using namespace Graphics;

Graphics::SpriteAnimation::LoadFromFile::LoadFromFile() :
    engine(nullptr), filePath()
{
}

SpriteAnimation::Frame::Frame() :
    textureView(), duration(0.0f)
{
}

SpriteAnimation::Frame::Frame(TextureView&& textureView, float duration) :
    textureView(std::move(textureView)), duration(duration)
{
}

SpriteAnimation::SpriteAnimation() :
    m_initialized(false)
{
}

SpriteAnimation::SpriteAnimation(SpriteAnimation&& other)
{
    *this = std::move(other);
}

SpriteAnimation& SpriteAnimation::operator=(SpriteAnimation&& other)
{
    std::swap(m_sequenceList, other.m_sequenceList);
    std::swap(m_sequenceMap, other.m_sequenceMap);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SpriteAnimation::Initialize()
{
    LOG() << "Initializing sprite animation..." << LOG_INDENT();

    // Make sure that this instance has not been already initialized.
    VERIFY(!m_initialized, "Sprite animation has already been initialized!");

    // Create a scope guard in case initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = SpriteAnimation());

    // Success!
    return m_initialized = true;
}

bool SpriteAnimation::Initialize(const LoadFromFile& params)
{
    LOG() << "Loading sprite animation from \"" << params.filePath << "\" file..." << LOG_INDENT();

    // Initialize the sprite animation instance.
    if(!this->Initialize())
        return false;

    // Create a scope guard in case initialization fails.
    bool initialized = false;

    SCOPE_GUARD_IF(!initialized, *this = SpriteAnimation());

    // Validate arguments.
    if(params.engine == nullptr)
    {
        LOG_ERROR() << "Invalid parameter - \"engine\" is null!";
        return false;
    }

    // Create a script state.
    Scripts::ScriptState::LoadFromFile scriptParams;
    scriptParams.engine = params.engine;
    scriptParams.filePath = params.filePath;

    Scripts::ScriptState scriptState;
    if(!scriptState.Initialize(scriptParams))
    {
        LOG_ERROR() << "Could not load file!";
        return false;
    }

    // Get the global table.
    lua_getglobal(scriptState, "SpriteAnimation");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR() << "Table \"SpriteAnimation\" is missing!";
        return false;
    }

    // Load the texture atlas.
    std::shared_ptr<TextureAtlas> textureAtlas;

    {
        lua_getfield(scriptState, -1, "TextureAtlas");
        SCOPE_GUARD(lua_pop(scriptState, 1));

        if(!lua_isstring(scriptState, -1))
        {
            LOG_ERROR() << "String \"SpriteAnimation\" is missing!";
            return false;
        }

        TextureAtlas::LoadFromFile textureAtlasParams;
        textureAtlasParams.engine = params.engine;
        textureAtlasParams.filePath = lua_tostring(scriptState, -1);

        textureAtlas = params.engine->resourceManager.Acquire<TextureAtlas>(
            textureAtlasParams.filePath, textureAtlasParams);

        if(!textureAtlas)
        {
            LOG_ERROR() << "Could not load texture atlas!";
            return false;
        }
    }

    // Read animation sequence.
    lua_getfield(scriptState, -1, "Sequences");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR() << "Table \"SpriteAnimation.Sequences\" is missing!";
        return false;
    }

    for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
    {
        // Check if the key is a string.
        if(!lua_isstring(scriptState, -2))
        {
            LOG_WARNING() << "Key in \"SpriteAnimation.Sequences\" is not string!";
            LOG_WARNING() << "Skipping one ill formated sequence!";
            continue;
        }

        std::string sequenceName = lua_tostring(scriptState, -2);

        // Read sequence frames.
        Sequence sequence;

        for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
        {
            // Make sure that we have a table.
            if(!lua_istable(scriptState, -1))
            {
                LOG_WARNING() << "Value in \"SpriteAnimation.Sequences[\"" << sequenceName << "\"]\" is not table!";
                LOG_WARNING() << "Skipping one ill formated frame!";
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
                    LOG_WARNING() << "Field in \"SpriteAnimation.Sequences[" << sequenceName << "][0]\" is not string!";
                    LOG_WARNING() << "Skipping one ill formated frame!";
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
                    LOG_WARNING() << "Field in \"SpriteAnimation.Sequences[\"" << sequenceName << "\"][1]\" is not number!";
                    LOG_WARNING() << "Skipping one ill formated frame!";
                    continue;
                }

                frameDuration = Utility::NumericalCast<float>(lua_tonumber(scriptState, -1));
            }

            // Add a frame to sequence.
            sequence.frames.emplace_back(std::move(textureView), frameDuration);
        }

        // Add sequence to the list.
        m_sequenceList.emplace_back(std::move(sequence));
        m_sequenceMap.emplace(sequenceName, m_sequenceList.size() - 1);
    }

    // Success!
    return initialized = true;
}

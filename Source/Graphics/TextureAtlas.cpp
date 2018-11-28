/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/TextureAtlas.hpp"
#include "Graphics/TextureView.hpp"
#include "Graphics/Texture.hpp"
#include "System/ResourceManager.hpp"
#include "Scripting/ScriptState.hpp"
#include "Engine/Root.hpp"
using namespace Graphics;

TextureAtlas::LoadFromFile::LoadFromFile() :
    engine(nullptr)
{
}

TextureAtlas::TextureAtlas() :
    m_texture(nullptr),
    m_initialized(false)
{
}

TextureAtlas::~TextureAtlas()
{
}

TextureAtlas::TextureAtlas(TextureAtlas&& other) :
    TextureAtlas()
{
    *this = std::move(other);
}

TextureAtlas& TextureAtlas::operator=(TextureAtlas&& other)
{
    std::swap(m_texture, other.m_texture);
    std::swap(m_regions, other.m_regions);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool TextureAtlas::Initialize()
{
    LOG() << "Initializing texture atlas..." << LOG_INDENT();

    // Make sure that instance has not been initialized yet.
    VERIFY(!m_initialized, "Texture atlas instance has already been initialized!");

    // Create a scoped guard in case initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = TextureAtlas());

    // Success!
    return m_initialized = true;
}

bool TextureAtlas::Initialize(const LoadFromFile& params)
{
    LOG() << "Loading texture atlas from \"" << params.filePath << "\" file..." << LOG_INDENT();

    // Make sure that instance has not been initialized yet.
    VERIFY(!m_initialized, "Texture atlas instance has already been initialized!");

    // Initialize texture atlas instance.
    if(!this->Initialize())
    {
        LOG_ERROR() << "Could not initialize texture atlas!";
        return false;
    }

    // Create a scoped guard in case initialization fails.
    bool initialized = false;

    SCOPE_GUARD_IF(!initialized, *this = TextureAtlas());

    // Validate parameters.
    if(params.engine == nullptr)
    {
        LOG_ERROR() << "Invalid parameter - \"engine\" is null!";
        return false;
    }

    // Load texture atlas from file using script state.
    Scripting::ScriptState::LoadFromFile scriptParams;
    scriptParams.engine = params.engine;
    scriptParams.filePath = params.filePath;

    Scripting::ScriptState scriptState;
    if(!scriptState.Initialize(scriptParams))
    {
        LOG_ERROR() << "Could not load file!";
        return false;
    }

    // Get the global table.
    lua_getglobal(scriptState, "TextureAtlas");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR() << "Table \"TextureAtlas\" is missing!";
        return false;
    }

    // Load the texture.
    {
        lua_getfield(scriptState, -1, "Texture");
        SCOPE_GUARD(lua_pop(scriptState, 1));

        if(!lua_isstring(scriptState, -1))
        {
            LOG_ERROR() << "String \"TextureAtlas.Texture\" is missing!";
            return false;
        }

        Texture::LoadFromFile textureParams;
        textureParams.engine = params.engine;
        textureParams.filePath = lua_tostring(scriptState, -1);
        textureParams.mipmaps = true;

        m_texture = params.engine->GetResourceManager().Acquire<Graphics::Texture>(
            textureParams.filePath, textureParams);

        if(m_texture == nullptr)
        {
            LOG_WARNING() << "Could not load texture!";
        }
    }

    // Read texture regions.
    lua_getfield(scriptState, -1, "Regions");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR() << "Table \"TextureAtlas.Regions\" is missing!";
        return false;
    }

    for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
    {
        // Check if the key is a string.
        if(!lua_isstring(scriptState, -2))
        {
            LOG_WARNING() << "Key in \"TextureAtlas.Regions\" is not string!";
            continue;
        }

        std::string regionName = lua_tostring(scriptState, -2);

        // Read the region rectangle.
        glm::ivec4 pixelCoords(0);

        for(int i = 0; i < 4; ++i)
        {
            lua_pushinteger(scriptState, i + 1);
            lua_gettable(scriptState, -2);

            if(!lua_isinteger(scriptState, -1))
            {
                LOG_WARNING() << "Value of \"TextureAtlas.Regions[\"" << regionName << "\"][" << i << "]\" is not an integer!";
            }

            pixelCoords[i] = Utility::NumericalCast<int>(lua_tointeger(scriptState, -1));

            lua_pop(scriptState, 1);
        }

        // Add a new texture region.
        if(!this->AddRegion(regionName, pixelCoords))
        {
            LOG_WARNING() << "Could not add region with \"" << regionName << "\" name!";
            continue;
        }
    }

    // Success!
    return initialized = true;
}

bool TextureAtlas::AddRegion(std::string name, glm::ivec4 pixelCoords)
{
    VERIFY(m_initialized, "Texture atlas has not been initialized yet!");

    // Add texture region to map.
    auto result = m_regions.emplace(name, pixelCoords);
    return result.second;
}

TextureView TextureAtlas::GetRegion(std::string name)
{
    VERIFY(m_initialized, "Texture atlas has not been initialized yet!");

    // Find and return texture region.
    auto it = m_regions.find(name);

    if(it != m_regions.end())
    {
        return TextureView(m_texture, it->second);
    }
    else
    {
        return TextureView();
    }
}

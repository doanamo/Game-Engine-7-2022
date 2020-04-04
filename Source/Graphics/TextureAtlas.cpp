/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/TextureAtlas.hpp"
#include "Graphics/TextureView.hpp"
#include "Graphics/Texture.hpp"
#include <System/FileSystem.hpp>
#include <System/ResourceManager.hpp>
#include <Script/ScriptState.hpp>
using namespace Graphics;

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
    LOG("Initializing texture atlas...");
    LOG_SCOPED_INDENT();

    // Make sure that instance has not been initialized yet.
    VERIFY(!m_initialized, "Texture atlas instance has already been initialized!");

    // Create a scoped guard in case initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = TextureAtlas());

    // Success!
    return m_initialized = true;
}

bool TextureAtlas::Initialize(const LoadFromFile& params)
{
    LOG("Loading texture atlas from \"{}\" file...", params.filePath);
    LOG_SCOPED_INDENT();

    // Make sure that instance has not been initialized yet.
    VERIFY(!m_initialized, "Texture atlas instance has already been initialized!");

    // Initialize texture atlas instance.
    if(!this->Initialize())
    {
        LOG_ERROR("Could not initialize texture atlas!");
        return false;
    }

    // Create a scoped guard in case initialization fails.
    bool initialized = false;

    SCOPE_GUARD_IF(!initialized, *this = TextureAtlas());

    // Validate parameters.
    if(params.fileSystem == nullptr)
    {
        LOG_ERROR("Invalid parameter - \"fileSystem\" is null!");
        return false;
    }

    if(params.resourceManager == nullptr)
    {
        LOG_ERROR("Invalid parameter - \"resourceManager\" is null!");
        return false;
    }

    // Load texture atlas from file using script state.
    Script::ScriptState::LoadFromFile scriptParams;
    scriptParams.fileSystem = params.fileSystem;
    scriptParams.filePath = params.filePath;

    Script::ScriptState scriptState;
    if(!scriptState.Initialize(scriptParams))
    {
        LOG_ERROR("Could not load file!");
        return false;
    }

    // Get the global table.
    lua_getglobal(scriptState, "TextureAtlas");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"TextureAtlas\" is missing!");
        return false;
    }

    // Load the texture.
    {
        lua_getfield(scriptState, -1, "Texture");
        SCOPE_GUARD(lua_pop(scriptState, 1));

        if(!lua_isstring(scriptState, -1))
        {
            LOG_ERROR("String \"TextureAtlas.Texture\" is missing!");
            return false;
        }

        Texture::LoadFromFile textureParams;
        textureParams.fileSystem = params.fileSystem;
        textureParams.filePath = lua_tostring(scriptState, -1);
        textureParams.mipmaps = true;

        m_texture = params.resourceManager->Acquire<Graphics::Texture>(textureParams.filePath, textureParams);

        if(m_texture == nullptr)
        {
            LOG_WARNING("Could not load texture!");
        }
    }

    // Read texture regions.
    lua_getfield(scriptState, -1, "Regions");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"TextureAtlas.Regions\" is missing!");
        return false;
    }

    for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
    {
        // Check if the key is a string.
        if(!lua_isstring(scriptState, -2))
        {
            LOG_WARNING("Key in \"TextureAtlas.Regions\" is not string!");
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
                LOG_WARNING("Value of \"TextureAtlas.Regions[\"{}\"][{}]\" is not an integer!", regionName, i);
            }

            pixelCoords[i] = Utility::NumericalCast<int>(lua_tointeger(scriptState, -1));

            lua_pop(scriptState, 1);
        }

        // Add a new texture region.
        if(!this->AddRegion(regionName, pixelCoords))
        {
            LOG_WARNING("Could not add region with \"{}\" name!", regionName);
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

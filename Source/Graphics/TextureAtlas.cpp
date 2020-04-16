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

TextureAtlas::TextureAtlas() = default;
TextureAtlas::~TextureAtlas() = default;

TextureAtlas::InitializeResult TextureAtlas::Initialize()
{
    LOG("Initializing texture atlas...");
    LOG_SCOPED_INDENT();

    // Success!
    m_initialized = true;
    return Success();
}

TextureAtlas::InitializeResult TextureAtlas::Initialize(const LoadFromFile& params)
{
    LOG("Loading texture atlas from \"{}\" file...", params.filePath);
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, this->Reset());

    // Call base initialization method.
    SUCCESS_OR_RETURN_RESULT(this->Initialize());

    // Validate parameters.
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.resourceManager != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Failure(InitializeErrors::InvalidArgument));

    // Load texture atlas from file using script state.
    Script::ScriptState::LoadFromFile scriptParams;
    scriptParams.fileSystem = params.fileSystem;
    scriptParams.filePath = params.filePath;

    Script::ScriptState scriptState;
    if(!scriptState.Initialize(scriptParams))
    {
        LOG_ERROR("Could not load file!");
        return Failure(InitializeErrors::FailedResourceLoading);
    }

    // Get the global table.
    lua_getglobal(scriptState, "TextureAtlas");
    SCOPE_GUARD(lua_pop(scriptState, 1));

    if(!lua_istable(scriptState, -1))
    {
        LOG_ERROR("Table \"TextureAtlas\" is missing!");
        return Failure(InitializeErrors::InvalidResourceContent);
    }

    // Load texture.
    {
        lua_getfield(scriptState, -1, "Texture");
        SCOPE_GUARD(lua_pop(scriptState, 1));

        if(!lua_isstring(scriptState, -1))
        {
            LOG_ERROR("String \"TextureAtlas.Texture\" is missing!");
            return Failure(InitializeErrors::InvalidResourceContent);
        }

        Texture::LoadFromFile textureParams;
        textureParams.fileSystem = params.fileSystem;
        textureParams.filePath = lua_tostring(scriptState, -1);
        textureParams.renderContext = params.renderContext;
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
        return Failure(InitializeErrors::InvalidResourceContent);
    }

    for(lua_pushnil(scriptState); lua_next(scriptState, -2); lua_pop(scriptState, 1))
    {
        // Check if key is a string.
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
    initialized = true;
    return Success();
}

bool TextureAtlas::AddRegion(std::string name, glm::ivec4 pixelCoords)
{
    VERIFY(m_initialized, "Texture atlas has not been initialized!");

    // Add texture region to map.
    auto result = m_regions.emplace(name, pixelCoords);
    return result.second;
}

TextureView TextureAtlas::GetRegion(std::string name)
{
    VERIFY(m_initialized, "Texture atlas has not been initialized!");

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

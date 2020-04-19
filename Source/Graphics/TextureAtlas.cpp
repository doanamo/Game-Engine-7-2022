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

TextureAtlas::CreateResult TextureAtlas::Create()
{
    LOG("Creating texture atlas...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<TextureAtlas>(new TextureAtlas());
    return Success(std::move(instance));
}

TextureAtlas::CreateResult TextureAtlas::Create(const LoadFromFile& params)
{
    LOG("Loading texture atlas from \"{}\" file...", params.filePath);
    LOG_SCOPED_INDENT();

    // Validate parameters.
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.resourceManager != nullptr, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Failure(CreateErrors::InvalidArgument));

    // Create base instance.
    auto createResult = Create();
    if(!createResult)
    {
        return createResult;
    }

    auto instance = createResult.Unwrap();

    // Load resource script.
    Script::ScriptState::LoadFromFile resourceParams;
    resourceParams.fileSystem = params.fileSystem;
    resourceParams.filePath = params.filePath;

    auto resourceScript = Script::ScriptState::Create(resourceParams).UnwrapOr(nullptr);
    if(resourceScript == nullptr)
    {
        LOG_ERROR("Could not load texture atlas resource file!");
        return Failure(CreateErrors::FailedResourceLoading);
    }

    // Get global table.
    lua_getglobal(*resourceScript, "TextureAtlas");
    SCOPE_GUARD(lua_pop(*resourceScript, 1));

    if(!lua_istable(*resourceScript, -1))
    {
        LOG_ERROR("Table \"TextureAtlas\" is missing!");
        return Failure(CreateErrors::InvalidResourceContent);
    }

    // Load texture.
    {
        lua_getfield(*resourceScript, -1, "Texture");
        SCOPE_GUARD(lua_pop(*resourceScript, 1));

        if(!lua_isstring(*resourceScript, -1))
        {
            LOG_ERROR("String \"TextureAtlas.Texture\" is missing!");
            return Failure(CreateErrors::InvalidResourceContent);
        }

        Texture::LoadFromFile textureParams;
        textureParams.fileSystem = params.fileSystem;
        textureParams.filePath = lua_tostring(*resourceScript, -1);
        textureParams.renderContext = params.renderContext;
        textureParams.mipmaps = true;

        instance->m_texture = params.resourceManager->Acquire<Graphics::Texture>(textureParams.filePath, textureParams);

        if(instance->m_texture == nullptr)
        {
            LOG_WARNING("Could not load texture!");
        }
    }

    // Read texture regions.
    lua_getfield(*resourceScript, -1, "Regions");
    SCOPE_GUARD(lua_pop(*resourceScript, 1));

    if(!lua_istable(*resourceScript, -1))
    {
        LOG_ERROR("Table \"TextureAtlas.Regions\" is missing!");
        return Failure(CreateErrors::InvalidResourceContent);
    }

    for(lua_pushnil(*resourceScript); lua_next(*resourceScript, -2); lua_pop(*resourceScript, 1))
    {
        // Check if key is a string.
        if(!lua_isstring(*resourceScript, -2))
        {
            LOG_WARNING("Key in \"TextureAtlas.Regions\" is not string!");
            continue;
        }

        std::string regionName = lua_tostring(*resourceScript, -2);

        // Read the region rectangle.
        glm::ivec4 pixelCoords(0);

        for(int i = 0; i < 4; ++i)
        {
            lua_pushinteger(*resourceScript, i + 1);
            lua_gettable(*resourceScript, -2);

            if(!lua_isinteger(*resourceScript, -1))
            {
                LOG_WARNING("Value of \"TextureAtlas.Regions[\"{}\"][{}]\" is not an integer!", regionName, i);
            }

            pixelCoords[i] = Utility::NumericalCast<int>(lua_tointeger(*resourceScript, -1));

            lua_pop(*resourceScript, 1);
        }

        // Add a new texture region.
        if(!instance->AddRegion(regionName, pixelCoords))
        {
            LOG_WARNING("Could not add region with \"{}\" name!", regionName);
            continue;
        }
    }

    // Success!
    return Success(std::move(instance));
}

bool TextureAtlas::AddRegion(std::string name, glm::ivec4 pixelCoords)
{
    // Add texture region to map.
    auto result = m_regions.emplace(name, pixelCoords);
    return result.second;
}

TextureView TextureAtlas::GetRegion(std::string name)
{
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

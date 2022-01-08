/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/TextureAtlas.hpp"
#include "Graphics/TextureView.hpp"
#include "Graphics/Texture.hpp"
#include <Core/System/SystemStorage.hpp>
#include <Platform/FileSystem/FileHandle.hpp>
#include <Core/Resource/ResourceManager.hpp>
using namespace Graphics;

namespace
{
    const char* LogCreateFailed = "Failed to create texture atlas from \"{}\" file! {}";
}

TextureAtlas::TextureAtlas() = default;
TextureAtlas::~TextureAtlas() = default;

TextureAtlas::CreateResult TextureAtlas::Create()
{
    LOG_PROFILE_SCOPE("Create texture atlas");

    // Create instance.
    auto instance = std::unique_ptr<TextureAtlas>(new TextureAtlas());
    return Common::Success(std::move(instance));
}

TextureAtlas::CreateResult TextureAtlas::Create(
    Platform::FileHandle& file, const LoadFromFile& params)
{
    LOG_PROFILE_SCOPE("Create texture atlas from \"{}\" file", file.GetPathString());
    LOG("Creating texture atlas from \"{}\" file...", file.GetPathString());

    CHECK_ARGUMENT_OR_RETURN(params.engineSystems, Common::Failure(CreateErrors::InvalidArgument));

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

    simdjson::ondemand::object textureAtlas;
    if(json.find_field("TextureAtlas").get(textureAtlas) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(), "Object \"TextureAtlas\" is missing.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    // Load texture.
    std::string_view texturePath;
    if(textureAtlas.find_field("Texture").get(texturePath) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(),
            "String \"TextureAtlas.Texture\" is missing.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    Texture::LoadFromFile textureParams;
    textureParams.engineSystems = params.engineSystems;
    textureParams.mipmaps = true;

    auto& resourceManager = params.engineSystems->Locate<Core::ResourceManager>();
    instance->m_texture = resourceManager.AcquireRelative<Graphics::Texture>(
        texturePath, file.GetPath(), textureParams).UnwrapEither();

    // Read texture regions.
    simdjson::ondemand::array regions;
    if(textureAtlas.find_field("Regions").get(regions) != simdjson::SUCCESS)
    {
        LOG_ERROR(LogCreateFailed, file.GetPathString(),
            "Array \"TextureAtlas.Regions\" is missing.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    uint32_t regionIndex = 0;
    for(auto region : regions)
    {
        std::string_view regionName;
        if(region.find_field("Name").get(regionName) != simdjson::SUCCESS)
        {
            LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                "String \"TextureAtlas.Regions[{}].Name\" is missing.",
                regionIndex));
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }

        simdjson::ondemand::array regionRect;
        if(region.find_field("Rect").get(regionRect) != simdjson::SUCCESS)
        {
            LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                "Array \"TextureAtlas.Regions[{}:{}].Rect\" is missing.",
                regionIndex, regionName));
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }

        uint32_t coordIndex = 0;
        glm::ivec4 imageCoords;
        for(auto coord : regionRect)
        {
            if(coordIndex >= 4)
            {
                LOG_WARNING("Array \"TextureAtlas.Regions[{}:{}].Rect\" contains more than 4 coordinates."
                    regionIndex, regionName);
                break;
            }

            int64_t value;
            if(coord.get(value) != simdjson::SUCCESS)
            {
                LOG_ERROR(LogCreateFailed, file.GetPathString(), fmt::format(
                    "Value \"TextureAtlas.Regions[{}:{}].Rect[{}]\" is not an integer.",
                    regionIndex, regionName, coordIndex));
                return Common::Failure(CreateErrors::FailedResourceLoading);
            }

            imageCoords[coordIndex++] = Common::NumericalCast<glm::ivec4::value_type>(value);
        }

        if(!instance->AddRegion(std::string(regionName), imageCoords))
        {
            LOG_ERROR(LogCreateFailed, file.GetPathString(),
                "Could not add region with \"{}\" name.", regionName);
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }

        ++regionIndex;
    }

    return Common::Success(std::move(instance));
}

bool TextureAtlas::AddRegion(std::string name, glm::ivec4 imageCoords)
{
    auto result = m_regions.emplace(std::move(name), imageCoords);
    return result.second;
}

TextureView TextureAtlas::GetRegion(std::string name)
{
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

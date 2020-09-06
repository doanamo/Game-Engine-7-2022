/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/RenderContext.hpp"
#include <System/FileSystem/FileHandle.hpp>
using namespace Graphics;

Texture::Texture() = default;

Texture::~Texture()
{
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteTextures(1, &m_handle);
        OpenGL::CheckErrors();
    }
}

Texture::CreateResult Texture::Create(const CreateFromParams& params)
{
    LOG("Creating texture...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.width > 0, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.height > 0, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.format != OpenGL::InvalidEnum, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<Texture>(new Texture());

    // Create texture handle.
    glGenTextures(1, &instance->m_handle);
    OpenGL::CheckErrors();

    if(instance->m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Texture could not be created!");
        return Common::Failure(CreateErrors::FailedTextureCreation);
    }

    // Bind texture.
    glBindTexture(GL_TEXTURE_2D, instance->m_handle);
    OpenGL::CheckErrors();

    SCOPE_GUARD(glBindTexture(GL_TEXTURE_2D,
        params.renderContext->GetState().GetTextureBinding(GL_TEXTURE_2D)));

    // Set packing alignment for provided data.
    if(params.format == GL_RED)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        OpenGL::CheckErrors();
    }

    SCOPE_GUARD(glPixelStorei(GL_UNPACK_ALIGNMENT,
        params.renderContext->GetState().GetPixelStore(GL_UNPACK_ALIGNMENT)));

    // Allocate texture surface on the hardware.
    glTexImage2D(GL_TEXTURE_2D, 0, params.format, params.width, params.height,
        0, params.format, GL_UNSIGNED_BYTE, params.data);

    // Generate texture mipmap.
    if(params.mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        OpenGL::CheckErrors();
    }

    // Save render context reference.
    instance->m_renderContext = params.renderContext;

    // Save texture parameters.
    instance->m_format = params.format;
    instance->m_width = params.width;
    instance->m_height = params.height;

    // Success!
    return Common::Success(std::move(instance));
}

Texture::CreateResult Texture::Create(System::FileHandle& file, const LoadFromFile& params)
{
    LOG("Loading texture from \"{}\" file...", file.GetPath());
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire engine services.
    Graphics::RenderContext* renderContext = params.services->GetRenderContext();

    // Validate file header.
    const size_t png_sig_size = 8;
    png_byte png_sig[png_sig_size];

    if(file.Read((uint8_t*)png_sig, png_sig_size) != png_sig_size)
    {
        LOG_ERROR("Could not read file header!");
        return Common::Failure(CreateErrors::FailedFileReading);
    }

    if(png_sig_cmp(png_sig, 0, png_sig_size) != 0)
    {
        LOG_ERROR("File path does not contain valid PNG file!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    // Create format decoder structures.
    png_structp png_read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(png_read_ptr == nullptr)
    {
        LOG_ERROR("Could not create PNG read structure!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    png_infop png_info_ptr = png_create_info_struct(png_read_ptr);
    if(png_info_ptr == nullptr)
    {
        LOG_ERROR("Could not create PNG info structure!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    SCOPE_GUARD_BEGIN();
    {
        png_destroy_read_struct(&png_read_ptr, &png_info_ptr, nullptr);
    }
    SCOPE_GUARD_END();

    // Declare file read function.
    auto png_read_function = [](png_structp png_ptr, png_bytep data, png_size_t length) -> void
    {
        System::FileHandle* file = (System::FileHandle*)png_get_io_ptr(png_ptr);
        if(file->Read((uint8_t*)data, length) != length)
        {
            png_error(png_ptr, "Unexpected end of file!");
        }
    };

    // Declare image buffers.
    png_bytep* png_row_ptrs = nullptr;
    png_byte* png_data_ptr = nullptr;

    SCOPE_GUARD_BEGIN();
    {
        delete[] png_row_ptrs;
        delete[] png_data_ptr;
    }
    SCOPE_GUARD_END();

    // Setup error handling routine.
    // Apparently a standard way of handling errors with libpng...
    // Library jumps here if one of its functions encounters an error!!!
    // This is the reason why scope guards and other objects are declared
    // before this call. Be aware of how dangerous it is to do in C++.
    // For e.g. objects created past this line will not have their
    // destructors called if the library jumps back here on an error.
    if(setjmp(png_jmpbuf(png_read_ptr)))
    {
        LOG_ERROR("Error occurred while reading file!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    // Setup the file read function.
    png_set_read_fn(png_read_ptr, (png_voidp)&file, png_read_function);

    // Set the amount of already read signature bytes.
    png_set_sig_bytes(png_read_ptr, png_sig_size);

    // Read image info.
    png_read_info(png_read_ptr, png_info_ptr);

    png_uint_32 width = png_get_image_width(png_read_ptr, png_info_ptr);
    png_uint_32 height = png_get_image_height(png_read_ptr, png_info_ptr);
    png_uint_32 depth = png_get_bit_depth(png_read_ptr, png_info_ptr);
    png_uint_32 channels = png_get_channels(png_read_ptr, png_info_ptr);
    png_uint_32 format = png_get_color_type(png_read_ptr, png_info_ptr);

    // Process different format types.
    switch(format)
    {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        if(depth < 8)
        {
            // Convert gray scale image to single 8bit channel.
            png_set_expand_gray_1_2_4_to_8(png_read_ptr);
            depth = 8;
        }
        break;

    case PNG_COLOR_TYPE_PALETTE:
        {
            // Convert indexed palette to RGB.
            png_set_palette_to_rgb(png_read_ptr);
            channels = 3;

            // Create alpha channel if palette has transparency.
            if(png_get_valid(png_read_ptr, png_info_ptr, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(png_read_ptr);
                channels += 1;
            }
        }
        break;

    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGBA:
        break;

    default:
        LOG_ERROR("Unsupported image format!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    // Make sure we only get 8bits per channel.
    if(depth == 16)
    {
        png_set_strip_16(png_read_ptr);
    }

    if(depth != 8)
    {
        LOG_ERROR("Unsupported image depth size!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    // Allocate image buffers.
    png_row_ptrs = new png_bytep[height];
    png_data_ptr = new png_byte[width * height * channels];

    // Setup an array of row pointers to the actual data buffer.
    png_uint_32 png_stride = width * channels;
    for(png_uint_32 i = 0; i < height; ++i)
    {
        // Reverse the order of rows to flip the image.
        // This is done because OpenGL's texture coordinates are also flipped.
        png_uint_32 png_row_index = height - i - 1;

        // Assemble an array of row pointers.
        png_uint_32 png_offset = i * png_stride;
        png_row_ptrs[png_row_index] = png_data_ptr + png_offset;
    }

    // Read image data.
    png_read_image(png_read_ptr, png_row_ptrs);

    // Determine the texture format.
    GLenum textureFormat = GL_NONE;

    switch(channels)
    {
    case 1:
        textureFormat = GL_RED;
        break;

    case 2:
        textureFormat = GL_RG;
        break;

    case 3:
        textureFormat = GL_RGB;
        break;

    case 4:
        textureFormat = GL_RGBA;
        break;

    default:
        LOG_ERROR("Unsupported number of channels!");
        return Common::Failure(CreateErrors::FailedPngLoading);
    }

    // Create instance.
    CreateFromParams createParams;
    createParams.renderContext = renderContext;
    createParams.width = width;
    createParams.height = height;
    createParams.format = textureFormat;
    createParams.mipmaps = params.mipmaps;
    createParams.data = png_data_ptr;
    return Create(createParams);
}

void Texture::Update(const void* data)
{
    VERIFY_ARGUMENT(data != nullptr);

    // Upload new texture data.
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, m_renderContext->GetState().GetTextureBinding(GL_TEXTURE_2D));
    OpenGL::CheckErrors();
}

GLuint Texture::GetHandle() const
{
    return m_handle;
}

int Texture::GetWidth() const
{
    return m_width;
}

int Texture::GetHeight() const
{
    return m_height;
}

/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/Image.hpp"
#include "System/FileSystem/FileHandle.hpp"
using namespace System;

Image::Image() = default;
Image::~Image() = default;

Image::CreateResult Image::Create(FileHandle& file, const LoadFromFile& params)
{
    LOG("Loading image from \"{}\" file...", file.GetPath().generic_string());
    LOG_SCOPED_INDENT();

    auto instance = std::unique_ptr<Image>(new Image());

    fs::path extension = file.GetPath().extension();
    if(extension == ".png")
    {
        if(auto failureResult = instance->LoadPNG(file).AsFailure())
        {
            return Common::Failure(failureResult.Unwrap());
        }
    }
    else
    {
        return Common::Failure(CreateErrors::UnknownExtension);
    }

    return Common::Success(std::move(instance));
}

Common::FailureResult<Image::CreateErrors> Image::LoadPNG(FileHandle& file)
{
    LOG("Loading PNG image from \"{}\" file...", file.GetPath().generic_string());
    LOG_SCOPED_INDENT();

    const size_t png_sig_size = 8;
    png_byte png_sig[png_sig_size];

    if(file.Read(png_sig, png_sig_size) != png_sig_size)
    {
        LOG_ERROR("Could not read file header!");
        return Common::Failure(CreateErrors::FailedFileRead);
    }

    if(png_sig_cmp(png_sig, 0, png_sig_size) != 0)
    {
        LOG_ERROR("File path does not contain valid PNG file!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    png_structp png_read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(png_read_ptr == nullptr)
    {
        LOG_ERROR("Could not create PNG read structure!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    SCOPE_GUARD([&png_read_ptr]
    {
        png_destroy_read_struct(&png_read_ptr, nullptr, nullptr);
    });

    png_infop png_info_ptr = png_create_info_struct(png_read_ptr);
    if(png_info_ptr == nullptr)
    {
        LOG_ERROR("Could not create PNG info structure!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    SCOPE_GUARD([&png_read_ptr, &png_info_ptr]
    {
        png_destroy_info_struct(png_read_ptr, &png_info_ptr);
    });

    auto png_read_function = [](png_structp png_ptr, png_bytep data, png_size_t length) -> void
    {
        auto* file = (System::FileHandle*)png_get_io_ptr(png_ptr);
        if(file->Read((uint8_t*)data, length) != length)
        {
            png_error(png_ptr, "Unexpected end of file!");
        }
    };

    png_bytep* png_row_ptrs = nullptr;

    SCOPE_GUARD([&png_row_ptrs]
    {
        delete[] png_row_ptrs;
    });

    // Setup error handling routine.
    // Apparently a standard way of handling errors with libpng.
    // Library jumps here if one of its functions encounters an error!
    // This is the reason why scope guards and other objects are declared
    // before this call. Be aware of how dangerous it is to do in C++.
    // For e.g. objects created past this line will not have their
    // destructor called if the library jumps back here on an error.
    if(setjmp(png_jmpbuf(png_read_ptr)))
    {
        LOG_ERROR("Error occurred while reading file!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    png_set_read_fn(png_read_ptr, (png_voidp)&file, png_read_function);
    png_set_sig_bytes(png_read_ptr, png_sig_size);
    png_read_info(png_read_ptr, png_info_ptr);

    png_uint_32 width = png_get_image_width(png_read_ptr, png_info_ptr);
    png_uint_32 height = png_get_image_height(png_read_ptr, png_info_ptr);
    png_uint_32 depth = png_get_bit_depth(png_read_ptr, png_info_ptr);
    png_uint_32 channels = png_get_channels(png_read_ptr, png_info_ptr);
    png_uint_32 format = png_get_color_type(png_read_ptr, png_info_ptr);

    switch(format)
    {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        if(depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(png_read_ptr);
            depth = png_get_bit_depth(png_read_ptr, png_info_ptr);
        }
        break;

    case PNG_COLOR_TYPE_PALETTE:
    {
        png_set_palette_to_rgb(png_read_ptr);
        channels = 3;

        if(png_get_valid(png_read_ptr, png_info_ptr, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(png_read_ptr);
            channels += 1;
        }

        ASSERT(png_get_channels(png_read_ptr, png_info_ptr) == channels);
    }
    break;

    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGBA:
        break;

    default:
        LOG_ERROR("Unsupported image format!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    if(depth == 16)
    {
        png_set_strip_16(png_read_ptr);
        depth = png_get_bit_depth(png_read_ptr, png_info_ptr);
    }

    if(depth != 8)
    {
        LOG_ERROR("Unsupported image depth size!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    m_data.resize(width * height* channels);
    static_assert(sizeof(Data::value_type) == sizeof(png_byte));

    png_row_ptrs = new png_bytep[height];
    png_uint_32 png_stride = width * channels;
    for(png_uint_32 i = 0; i < height; ++i)
    {
        // Reverse order of rows to flip image because
        // OpenGL's texture coordinates are also flipped.
        png_uint_32 png_row_index = height - i - 1;
        png_uint_32 png_offset = i * png_stride;
        png_row_ptrs[png_row_index] = m_data.data() + png_offset;
    }

    png_read_image(png_read_ptr, png_row_ptrs);

    if(m_data.empty())
    {
        LOG_ERROR("Failed to ready any image data from file!");
        return Common::Failure(CreateErrors::FailedPngLoad);
    }

    m_width = width;
    m_height = height;
    m_channels = channels;

    return Common::Success();
}

const uint8_t* Image::GetData() const
{
    return m_data.data();
}

int Image::GetWidth() const
{
    return m_width;
}

int Image::GetHeight() const
{
    return m_height;
}

int Image::GetChannels() const
{
    return m_channels;
}

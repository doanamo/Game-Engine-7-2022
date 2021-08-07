/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Image

    Loads image data from arbitrary formats.
*/

namespace System
{
    class FileHandle;

    class Image final
    {
    public:
        struct LoadFromFile
        {
        };

        enum class CreateErrors
        {
            InvalidArgument,
            UnknownExtension,
            FailedFileRead,
            FailedPngLoad,
        };

        using CreateResult = Common::Result<std::unique_ptr<Image>, CreateErrors>;
        static CreateResult Create(FileHandle& file, const LoadFromFile& params);

        using Data = std::vector<uint8_t>;

    public:
        ~Image();

        const uint8_t* GetData() const
        {
            return m_data.data();
        }

        int GetWidth() const
        {
            return m_width;
        }

        int GetHeight() const
        {
            return m_height;
        }

        int GetChannels() const
        {
            return m_channels;
        }

    private:
        Image();

        Common::FailureResult<CreateErrors> LoadPNG(FileHandle& file);

    private:
        Data m_data;
        int m_width = 0;
        int m_height = 0;
        int m_channels = 0;
    };
}

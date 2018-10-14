/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/TextureView.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Sprite Animation
*/

namespace Graphics
{
    // Sprite animation class.
    class SpriteAnimation
    {
    public:
        // Loads sprite animation from a file.
        struct LoadFromFile
        {
            LoadFromFile();

            Engine::Root* engine;
            std::string filePath;
        };

        // Sprite frame structure.
        struct Frame
        {
            Frame();
            Frame(TextureView&& textureView, float duration);

            TextureView textureView;
            float duration;
        };

        // Animation sequence structure.
        struct Sequence
        {
            std::vector<Frame> frames;
        };

        // List of sequences.
        using SequenceList = std::vector<Sequence>;
        using SequenceMap = std::unordered_map<std::string, std::size_t>;

    public:
        SpriteAnimation();

        SpriteAnimation(const SpriteAnimation& other) = delete;
        SpriteAnimation& operator=(const SpriteAnimation& other) = delete;

        SpriteAnimation(SpriteAnimation&& other);
        SpriteAnimation& operator=(SpriteAnimation&& other);

        // Initializes the sprite animation instance.
        bool Initialize();

        // Loads the sprite animation from a file.
        bool Initialize(const LoadFromFile& params);

    private:
        // Animation sequences.
        SequenceList m_sequenceList;
        SequenceMap m_sequenceMap;

        // Initialization state.
        bool m_initialized;
    };
}

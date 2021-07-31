/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Sprite

    Structure that defines a textured quad. Consists of two parts - information that can be shared
    between different instances of sprites and data that is unique for each sprite. This is done to
    support efficient sprite sorting and rendering.
*/

namespace Graphics
{
    class Texture;

    struct Sprite
    {
        struct Info
        {
            // Comparison operators used for sorting.
            bool operator==(const Info& other) const;
            bool operator!=(const Info& other) const;

            // Shared info defined per sprite batch.
            const Texture* texture = nullptr;
            bool transparent = false;
            bool filtered = true;
        } info;

        struct Data
        {
            // Sprite data defined per sprite instance.
            glm::mat4 transform = glm::mat4(1.0f);
            glm::vec4 rectangle = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            glm::vec4 coords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        } data;
    };
}

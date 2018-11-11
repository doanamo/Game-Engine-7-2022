/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Sprite

    Structure that defines a textured quad. Consists of two parts - information
    that can be shared between different instances of sprites and data that is
    unique for each sprite. This is done to support efficient sprite sorting
    and rendering.
*/

namespace Graphics
{
    // Forward declarations.
    class Texture;

    // Sprite structure.
    struct Sprite
    {
        // Info structure defined per sprite batch.
        struct Info
        {
            Info();

            // Comparison operators used for sorting.
            bool operator==(const Info& other) const;
            bool operator!=(const Info& other) const;

            // Shared batch info.
            const Texture* texture;
            bool transparent;
            bool filtered;
        } info;

        // Data structure defined per sprite instance.
        struct Data
        {
            Data();

            // Sprite data.
            glm::mat4 transform;
            glm::vec4 rectangle;
            glm::vec4 coords;
            glm::vec4 color;
        } data;
    };
}

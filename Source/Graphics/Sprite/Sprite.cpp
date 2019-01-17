/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Sprite/Sprite.hpp"
using namespace Graphics;

Sprite::Info::Info() :
    texture(nullptr),
    transparent(false),
    filtered(true)
{
}

bool Sprite::Info::operator==(const Info& other) const
{
    return texture == other.texture && transparent == other.transparent && filtered == other.filtered;
}

bool Sprite::Info::operator!=(const Info& other) const
{
    return texture != other.texture || transparent != other.transparent || filtered != other.filtered;
}

Sprite::Data::Data() :
    transform(1.0f),
    rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    coords(0.0f, 0.0f, 1.0f, 1.0f),
    color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

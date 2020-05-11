/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "../Precompiled.hpp"
#include "Graphics/Sprite/Sprite.hpp"
using namespace Graphics;

bool Sprite::Info::operator==(const Info& other) const
{
    return texture == other.texture && transparent == other.transparent && filtered == other.filtered;
}

bool Sprite::Info::operator!=(const Info& other) const
{
    return texture != other.texture || transparent != other.transparent || filtered != other.filtered;
}

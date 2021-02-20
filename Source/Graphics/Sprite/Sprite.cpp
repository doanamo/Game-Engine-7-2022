/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
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

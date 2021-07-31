/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/ScreenSpace.hpp"
using namespace Graphics;

ScreenSpace::ScreenSpace() = default;
ScreenSpace::~ScreenSpace() = default;

void ScreenSpace::SetTargetSize(int width, int height)
{
    m_targetSize.x = (float)width;
    m_targetSize.y = (float)height;

    m_rebuild = true;
}

void ScreenSpace::SetSourceSize(float width, float height)
{
    m_sourceSize.x = width;
    m_sourceSize.y = height;
    m_sourceAspectRatio = width / height;

    m_rebuildSourceSize = false;
    m_rebuild = true;
}

void ScreenSpace::SetSourceAspectRatio(float aspectRatio)
{
    m_sourceSize.x = 0.0f;
    m_sourceSize.y = 0.0f;
    m_sourceAspectRatio = aspectRatio;

    m_rebuildSourceSize = true;
    m_rebuild = true;
}

void ScreenSpace::Rebuild() const
{
    if(m_rebuild)
    {
        // Rebuild source size if needed.
        if(m_rebuildSourceSize)
        {
            // Set source size equal to target size before adjusting aspect ratio.
            m_sourceSize = m_targetSize;

            // Adjust aspect ratio of source size.
            float targetAspectRatio = m_targetSize.x / m_targetSize.y;

            if(targetAspectRatio > m_sourceAspectRatio)
            {
                m_sourceSize.x = m_targetSize.x / (targetAspectRatio / m_sourceAspectRatio);
            }
            else
            {
                m_sourceSize.y = m_targetSize.y * (targetAspectRatio / m_sourceAspectRatio);
            }   
        }

        // Calculate aspect ratios.
        float targetAspectRatio = m_targetSize.x / m_targetSize.y;
        float sourceAspectRatio = m_sourceSize.x / m_sourceSize.y;
        float aspectRatio = targetAspectRatio / sourceAspectRatio;

        // Calculate screen space coordinates.
        m_coords.x = -m_sourceSize.x * 0.5f; // Left
        m_coords.y =  m_sourceSize.x * 0.5f; // Right
        m_coords.z = -m_sourceSize.y * 0.5f; // Bottom
        m_coords.w =  m_sourceSize.y * 0.5f; // Top

        // Scale screen space coordinates.
        if(targetAspectRatio > sourceAspectRatio)
        {
            m_coords.x *= aspectRatio;
            m_coords.y *= aspectRatio;
        }
        else
        {
            m_coords.z /= aspectRatio;
            m_coords.w /= aspectRatio;
        }

        // Calculate screen space offset.
        m_offset.x = -m_sourceSize.x * 0.5f;
        m_offset.y = -m_sourceSize.y * 0.5f;

        // Calculate screen space matrices.
        m_projection = glm::ortho(m_coords.x, m_coords.y, m_coords.z, m_coords.w);
        m_view = glm::translate(glm::mat4(1.0f), glm::vec3(m_offset, 0.0f));
        m_transform = m_projection * m_view;

        m_rebuild = false;
    }
}

const glm::vec4& ScreenSpace::GetVisibleSourceExtents() const
{
    Rebuild();
    return m_coords;
}

const glm::vec2& ScreenSpace::GetOffsetFromCenter() const
{
    Rebuild();
    return m_offset;
}

const glm::mat4& ScreenSpace::GetProjection() const
{
    Rebuild();
    return m_projection;
}

const glm::mat4& ScreenSpace::GetView() const
{
    Rebuild();
    return m_view;
}

const glm::mat4& ScreenSpace::GetTransform() const
{
    Rebuild();
    return m_transform;
}

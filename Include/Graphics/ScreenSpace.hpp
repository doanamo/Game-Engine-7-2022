/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Screen Space
    
    Creates a screen space transform with an orthogonal projection that can be used to
    create a self maintaining drawing space that is independent of the target resolution.

    WTTTTSSSSSSSSSSSSSTTTTT
    T    S           S    T
    T    S           S    T
    T    S     C     S    T
    T    S           S    T
    T    S           S    T
    GTTTTOSSSSSSSSSSSSTTTTT

    S - Source Size
    T - Target Size
    C - Source Center
    O - Source Origin
    W - Window Origin
    G - OpenGL Origin
    
    Requires source size (i.e. world coordinates) and target size (e.g. viewport or window)
    to be specified. The source will be always enclosed inside the target so it remains fully
    visible. It will also be scaled if needed to maintain largest possible size and aspect ratio
    if the enclosing target becomes smaller or larger.
    
    void ExampleGraphicsScreenSpace()
    {
        // Create screen space instance.
        Graphics::ScreenSpace screenSpace;
        screenSpace.SetSourceSize(1024.0f, 576.0f);
        
        // Main rendering loop.
        while(true)
        {
            // Update target size.
            m_screenSpace.SetTargetSize(windowWidth, windowHeight);

            // Setup viewport and camera transform.
            glViewport(0, 0, windowWidth, windowHeight);
            glm::mat4 transform = screenSpace.GetTransform();
        }
    }
*/

namespace Graphics
{
    class ScreenSpace final : public Resettable<ScreenSpace>
    {
    public:
        ScreenSpace();
        ~ScreenSpace();

        // Needs to be set once to define maintained screen space size.
        void SetSourceSize(float width, float height);

        // Same as SetSourceSize() but maintains screen space to match only the aspect ratio.
        // Aspect ratio is equal to horizontal width divided by vertical height.
        void SetSourceAspectRatio(float aspectRatio);

        // Needs to be updated every time the target resizes.
        void SetTargetSize(int width, int height);

        const glm::vec2& GetSourceSize() const;
        const glm::vec2& GetTargetSize() const;
        const glm::vec4& GetVisibleSourceExtents() const;
        const glm::vec2& GetOffsetFromCenter() const;

        const glm::mat4& GetProjection() const;
        const glm::mat4& GetView() const;
        const glm::mat4& GetTransform() const;

    private:
        mutable float m_sourceAspectRatio = 1.0f;
        mutable glm::vec2 m_targetSize = glm::vec2(2.0f, 2.0f);
        mutable glm::vec2 m_sourceSize = glm::vec2(2.0f, 2.0f);

        mutable glm::vec4 m_coords = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        mutable glm::vec2 m_offset = glm::vec2(0.0f, 0.0f);

        mutable glm::mat4 m_projection = glm::mat4(1.0f);
        mutable glm::mat4 m_view = glm::mat4(1.0f);
        mutable glm::mat4 m_transform = glm::mat4(1.0f);

    private:
        void Rebuild() const;

        mutable bool m_rebuild = true;
        mutable bool m_rebuildSourceSize = false;
    };
}

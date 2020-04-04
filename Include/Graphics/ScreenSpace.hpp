/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Screen Space
    
    Creates a screen space transform with an orthogonal projection that can be used to
    create a self maintaining drawing space that is independant of the target resolution.

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
        // Create a screen space instance.
        Graphics::ScreenSpace screenSpace;
        screenSpace.SetSourceSize(1024.0f, 576.0f);
        
        // Main rendering loop.
        while(true)
        {
            // Update target size.
            m_screenSpace.SetTargetSize(windowWidth, windowHeight);

            // Setup a viewport and camera transform.
            glViewport(0, 0, windowWidth, windowHeight);
            glm::mat4 transform = screenSpace.GetTransform();
        }
    }
*/

namespace Graphics
{
    // Screen space class.
    class ScreenSpace : private NonCopyable
    {
    public:
        ScreenSpace();
        ~ScreenSpace();

        // Move constructor and assignment.
        ScreenSpace(ScreenSpace&& other);
        ScreenSpace& operator=(ScreenSpace&& other);

        // Sets the source size.
        // Needs to be set once to define maintained screen space size.
        void SetSourceSize(float width, float height);

        // Sets the source size aspect ratio instead of specifying size.
        // Same as SetSourceSize() but maintains screen space to match only the aspect ratio.
        // Aspect ratio is equal to horizontal width divided by vertical height.
        void SetSourceAspectRatio(float aspectRatio);

        // Sets the target size.
        // Needs to be updated every time the target resizes.
        void SetTargetSize(int width, int height);

        // Gets the source size.
        const glm::vec2& GetSourceSize() const;

        // Gets the target size.
        const glm::vec2& GetTargetSize() const;

        // Gets the visible range of source coordinates from the center of the target.
        // Returns a rectangle [left, right, bottom, top] with offsets from the center to each side.
        const glm::vec4& GetVisibleSourceExtents() const;

        // Gets the offset from the center to bottom left corner of the maintained screen space.
        // Can be used to move the origin to the center of the view.
        const glm::vec2& GetOffsetFromCenter() const;

        // Gets the projection matrix.
        const glm::mat4& GetProjection() const;

        // Gets the view matrix.
        const glm::mat4& GetView() const;

        // Gets the combined projection and view matrices.
        const glm::mat4& GetTransform() const;

    private:
        // Target size.
        mutable glm::vec2 m_targetSize;

        // Source size.
        mutable glm::vec2 m_sourceSize;
        mutable float m_sourceAspectRatio;

        // View parameters.
        mutable glm::vec4 m_coords;
        mutable glm::vec2 m_offset;

        // View transform.
        mutable glm::mat4 m_projection;
        mutable glm::mat4 m_view;
        mutable glm::mat4 m_transform;

    private:
        // Rebuilds matrices.
        void Rebuild() const;

        // Rebuild flags.
        mutable bool m_rebuild;
        mutable bool m_rebuildSourceSize;
    };
}

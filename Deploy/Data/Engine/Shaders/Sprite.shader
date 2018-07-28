#version 330

#if defined(VERTEX_SHADER)
    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 vertexTexture;
    layout(location = 2) in mat4 instanceTransform;
    layout(location = 6) in vec4 instanceRectangle;
    layout(location = 7) in vec4 instanceColor;

    out vec2 fragmentTexture;
    out vec4 fragmentColor;

    uniform mat4 viewTransform;
    uniform vec2 textureSizeInv;

    void main()
    {
        vec4 position = vec4(vertexPosition, 0.0f, 1.0f);
        vec2 texture = vertexTexture;

        // Scale vertex position by sprite size.
        // Size can be negative for mirrored sprites.
        position.xy *= abs(instanceRectangle.zw);

        // Apply transformation.
        position = instanceTransform * position;
        position = viewTransform * position;

        // Normalize texture coordinates.
        texture *= instanceRectangle.zw * textureSizeInv;
        texture += instanceRectangle.xy * textureSizeInv;

        // Move texture origin from top left corner to bottom left.
        texture.y -= instanceRectangle.w * textureSizeInv.y;

        // Output a sprite vertex.
        gl_Position = position;
        fragmentTexture = texture;
        fragmentColor = instanceColor;
    }
#endif

#if defined(FRAGMENT_SHADER)
    in  vec2 fragmentTexture;
    in  vec4 fragmentColor;
    out vec4 finalColor;

    uniform sampler2D textureDiffuse;

    void main()
    {
        finalColor = texture(textureDiffuse, fragmentTexture) * fragmentColor;
    }
#endif

#version 330

#if defined(VERTEX_SHADER)
    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 vertexTexture;
    layout(location = 2) in mat4 instanceTransform;
    layout(location = 6) in vec4 instanceSpriteRectangle;
    layout(location = 7) in vec4 instanceTextureRectangle;
    layout(location = 8) in vec4 instanceColor;

    out vec2 fragmentTexture;
    out vec4 fragmentColor;

    uniform mat4 vertexTransform;
    uniform vec2 textureSizeInv;

    void main()
    {
        // Calculate sprite rectangle size.
        vec2 spriteRectangleSize;

        spriteRectangleSize.x = instanceSpriteRectangle.z - instanceSpriteRectangle.x;
        spriteRectangleSize.y = instanceSpriteRectangle.w - instanceSpriteRectangle.y;

        // Transform basic sprite vertex by sprite rectangle.
        vec4 position = vec4(vertexPosition, 0.0f, 1.0f);

        position.xy *= spriteRectangleSize;
        position.xy += instanceSpriteRectangle.xy;

        // Apply vertex transformations.
        position = instanceTransform * position;
        position = vertexTransform * position;

        // Calculate texture rectangle size.
        vec2 textureRectangleSize;

        textureRectangleSize.x = instanceTextureRectangle.z - instanceTextureRectangle.x;
        textureRectangleSize.y = instanceTextureRectangle.w - instanceTextureRectangle.y;

        // Transform basic sprite texture coords by texture rectangle.
        vec2 textureCoords = vertexTexture;

        textureCoords *= textureRectangleSize * textureSizeInv;
        textureCoords += instanceTextureRectangle.xy * textureSizeInv;

        // Move texture origin from top left corner to bottom left.
        textureCoords.y -= textureRectangleSize.y * textureSizeInv.y;

        // Output a sprite vertex.
        gl_Position = position;
        fragmentTexture = textureCoords;
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

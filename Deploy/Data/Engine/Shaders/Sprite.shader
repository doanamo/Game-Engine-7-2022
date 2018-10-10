#version 330

/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;

    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 vertexCoords;
    layout(location = 2) in mat4 instanceTransform;
    layout(location = 6) in vec4 instanceRectangle;
    layout(location = 7) in vec4 instanceCoords;
    layout(location = 8) in vec4 instanceColor;

    out vec2 fragmentCoords;
    out vec4 fragmentColor;

    void main()
    {
        // Transform base quad using sprite rectangle.
        vec4 position = vec4(vertexPosition, 0.0f, 1.0f);

        position.x *= instanceRectangle.z - instanceRectangle.x;
        position.y *= instanceRectangle.w - instanceRectangle.y;

        position.x += instanceRectangle.x;
        position.y += instanceRectangle.y;

        // Transform position using vertex and instance transformations.
        position = instanceTransform * position;
        position = vertexTransform * position;

        // Transform base coordinates using texture rectangle.
        vec2 coords = vertexCoords;

        coords.x *= instanceCoords.z - instanceCoords.x;
        coords.y *= instanceCoords.w - instanceCoords.y;

        coords.x += instanceCoords.x;
        coords.y += instanceCoords.y;

        // Output a sprite vertex.
        gl_Position = position;
        fragmentCoords = coords;
        fragmentColor = instanceColor;
    }
#endif

#if defined(FRAGMENT_SHADER)
    in  vec2 fragmentCoords;
    in  vec4 fragmentColor;
    out vec4 finalColor;

    uniform sampler2D textureDiffuse;

    void main()
    {
        // Output a fragment color.
        finalColor = texture(textureDiffuse, fragmentCoords) * fragmentColor;
    }
#endif

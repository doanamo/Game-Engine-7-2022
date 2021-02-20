#version 300 es

/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;

    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 vertexTexture;
    layout(location = 2) in vec4 vertexColor;
    
    out vec2 fragmentTexture;
    out vec4 fragmentColor;

    void main()
    {
        gl_Position = vertexTransform * vec4(vertexPosition, 0.0f, 1.0f);
        fragmentTexture = vertexTexture;
        fragmentColor = vertexColor;
    }
#endif

#if defined(FRAGMENT_SHADER)
    precision mediump float;

    in vec2 fragmentTexture;
    in vec4 fragmentColor;
    out vec4 finalColor;

    uniform sampler2D textureDiffuse;

    void main()
    {
        finalColor = texture(textureDiffuse, fragmentTexture) * fragmentColor;
    }
#endif

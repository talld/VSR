#version 450
layout(set = 0, binding = 0) uniform sampler2D textures[1];

layout(location = 0) in  vec2 inUV;
layout(location = 0) out vec4 outColor;

void main()
{
    vec2 newUV = vec2(inUV.x, 1 - inUV.y);

    outColor = texture(textures[0], newUV);
}
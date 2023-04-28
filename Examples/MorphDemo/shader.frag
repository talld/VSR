#version 450
layout(set = 0, binding = 0) uniform sampler2D textures[256];

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in flat uint inIndex;
layout(location = 4) in mat4 inModelMatrix;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 col = texture(textures[inIndex], inUV);
    outColor = vec4(
        col.x,
        col.y,
        col.z,
        col.w
    ) ;
}
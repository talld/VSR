#version 450
layout(set = 0, binding = 0) uniform sampler2D textures[256];

struct Light
{
    vec3 pos;
    uint r, g, b, a;
};

layout(set = 1, binding = 0) buffer LIGHTS
{
    Light list[4];
} Lights;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in flat uint inIndex;
layout(location = 4) in mat4 inModelMatrix;

layout(location = 0) out vec4 outColor;

void main()
{

    outColor = vec4(
        inPos.z,
        0,
        0,
        255
    ) ;
}
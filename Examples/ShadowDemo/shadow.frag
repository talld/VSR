#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in flat uint inIndex;
layout(location = 4) in mat4 inModelMatrix;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
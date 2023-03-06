#version 450

//push constants
layout(push_constant) uniform constants
{
    mat4 mvp;
} PushConstants;

layout(location = 0) in vec3 xyz;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;

void main()
{
    outUV = inUV;
    gl_Position =  PushConstants.mvp * vec4(xyz, 1.0);
}
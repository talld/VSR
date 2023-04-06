#version 450

//push constants
layout(push_constant) uniform constants
{
    mat4 vp;
    int arr[24];
}
PushConstants;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in uint inIndex;
layout(location = 4) in mat4 inModelMatrix;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out flat uint outIndex;
layout(location = 4) out mat4 outModelMatrix;

void main()
{
    outPos = inPos;
    outNormal = inNormal;
    outUV = inUV;
    outIndex = inIndex;
    outModelMatrix = inModelMatrix;

    gl_Position = PushConstants.vp * inModelMatrix * vec4(inPos, 1.0f);
}
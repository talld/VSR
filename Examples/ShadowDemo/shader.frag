#version 450
#extension GL_EXT_debug_printf : enable
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

layout(location = 8) in vec4 shadowCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 shadowMapCoords  = (shadowCoord.xyz / shadowCoord.w);
    shadowMapCoords = (shadowMapCoords * 0.5) + 0.5;
    debugPrintfEXT("shadowMapCoords: %v3f shadowCoord: %v4f",
                   shadowMapCoords, shadowCoord);

    float closestDepth = texture(textures[0], shadowMapCoords.xy).z;
    float currentDepth = shadowMapCoords.z;
    debugPrintfEXT("closestDepth: %f currentDepth: %f", closestDepth, currentDepth);
    float fragmentVisibility = currentDepth > closestDepth ? 0.0 : 1.0;

    vec3 toLight = normalize( vec3(vec4(Lights.list[0].pos,1) * inModelMatrix) - inPos);
    vec4 col = texture(textures[inIndex], inUV);
    vec4 normal = normalize(texture(textures[inIndex+1], inUV) );

    float diff = dot(normalize(vec3(normal) *  inNormal), toLight);
    diff = max(diff, 0.0);

    outColor = vec4(
        col.x * fragmentVisibility,
        col.y * fragmentVisibility,
        col.z * fragmentVisibility,
        col.w
    );
}
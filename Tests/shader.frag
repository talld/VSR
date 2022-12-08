#version 450

layout(location = 0) in  vec3 fragColour;
layout(location = 0) out vec4 outColor;

void main()
{
    vec3 col = fragColour * gl_FragCoord.z;
    outColor = vec4(col, 1.f);
}
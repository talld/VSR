#version 450

layout(location = 0) in vec3 xyz;
layout(location = 1) in vec2 UV;

layout(location = 0) out vec3 fragColour;


//push constants
layout(push_constant) uniform constants
{
   mat4 mvp;
} PushConstants;

vec3 convertHSV2RGB(vec3 HSV)
{
    float H = abs( HSV.x * 360 );
    float S = abs( HSV.y * 100 );
    float V = 100;

    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-abs(mod(H/60.0, 2)-1));
    float m = v-C;

    float r,g,b;

    if(H >= 0 && H < 60)
    {
        r = C;
        g = X;
        b = 0;
    }
    else if(H >= 60 && H < 120)
    {
        r = X;
        g = C;
        b = 0;
    }
    else if(H >= 120 && H < 180)
    {
        r = 0;
        g = C;
        b = X;
    }
    else if(H >= 180 && H < 240)
    {
        r = 0;
        g = X;
        b = C;
    }
    else if(H >= 240 && H < 300)
    {
        r = X;
        g = 0;
        b = C;
    }
    else
    {
        r = C;
        g = 0;
        b = X;
    }

    return vec3((r+m),(g+m),(b+m));
}

void main()
{
    vec3 HSV = vec3(UV,1);
    fragColour = convertHSV2RGB(HSV);

    gl_Position =  PushConstants.mvp * vec4(xyz, 1.0);
}
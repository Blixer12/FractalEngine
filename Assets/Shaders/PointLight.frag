#version 450

layout(location=0)in vec2 FragOffset;
layout(location=0)out vec4 Color;

layout(set=0,binding=0)uniform GlobalUBO
{
    mat4 Projection;
    mat4 View;
    
    vec4 AmbientLightColor;// The 4th Component is Intensity
    vec4 LightPosition;// The 4th Component is Radius
    vec4 LightColor;// The 4th Component is Intensity
}UBO;

void main()
{
    Color=vec4(UBO.LightColor.rgb,1.);
}
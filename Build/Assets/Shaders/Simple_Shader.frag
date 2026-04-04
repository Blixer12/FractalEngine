#version 450

layout(location=0)in vec4 Color;
layout(location=1)in vec3 PositionWorldSpace;
layout(location=2)in vec3 NormalWorldSpace;

layout(location=0)out vec4 ColorOutput;

layout(set=0,binding=0)uniform GlobalUBO
{
    mat4 Projection;
    mat4 View;
    
    vec4 AmbientLightColor;// The 4th Component is Intensity
    vec4 LightPosition;// The 4th Component is Radius
    vec4 LightColor;// The 4th Component is Intensity
}UBO;

layout(push_constant)uniform PushConstant
{
    mat4 ModelMatrix;
    mat4 NormalMatrix;
}Push;

void main()
{
    
    vec3 DirectionToLight=UBO.LightPosition.xyz-PositionWorldSpace;
    float DistanceToLight=1./dot(DirectionToLight,DirectionToLight);
    
    vec3 LightColor=UBO.LightColor.xyz*UBO.LightColor.w*DistanceToLight;
    vec3 AmbientLight=UBO.AmbientLightColor.xyz*UBO.AmbientLightColor.w;
    vec3 DiffuseLight=LightColor*max(dot(normalize(NormalWorldSpace),normalize(DirectionToLight)),0);
    
    ColorOutput=vec4((DiffuseLight+AmbientLight)*Color.rgb,Color.a);
}
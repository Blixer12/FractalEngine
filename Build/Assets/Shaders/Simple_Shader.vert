#version 450

layout(location=0)in vec3 Position;
layout(location=1)in vec3 Normal;
layout(location=2)in vec2 UV;
layout(location=3)in vec4 Color;

layout(location=0)out vec4 FragColor;
layout(location=1)out vec3 FragWorldPosition;
layout(location=2)out vec3 FragWorldNormal;

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
    vec4 PositionWorldSpace=Push.ModelMatrix*vec4(Position,1);
    gl_Position=UBO.Projection*UBO.View*PositionWorldSpace;
    
    FragWorldPosition=PositionWorldSpace.xyz;
    FragWorldNormal=normalize(mat3(Push.NormalMatrix)*Normal);
    FragColor=Color;
}
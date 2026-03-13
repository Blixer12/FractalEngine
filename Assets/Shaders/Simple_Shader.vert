#version 450

layout(location=0)in vec2 Position;
layout(location=1)in vec4 Color;

layout(push_constant)uniform PushConstant
{
    mat2 Transform;
    vec4 Color;
    vec2 Offset;
}Push;

void main()
{
    gl_Position=vec4(Push.Transform*Position+Push.Offset,0,1);
}
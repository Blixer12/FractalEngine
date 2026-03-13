#version 450

layout(location=0)out vec4 ColorOutput;

layout(push_constant)uniform PushConstant
{
    mat2 Transform;
    vec4 Color;
    vec2 Offset;
}Push;

void main()
{
    ColorOutput=vec4(Push.Color);
}
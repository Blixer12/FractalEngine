#version 450

layout(location=0)in vec4 Color;

layout(location=0)out vec4 ColorOutput;

void main()
{
    ColorOutput=vec4(Color);
}
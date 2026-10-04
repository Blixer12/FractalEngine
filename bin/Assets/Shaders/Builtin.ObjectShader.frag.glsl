#version 460

layout(location = 0) in vec3 InPosition;

layout(location = 0) out vec4 OutColor;

void main()
{
    OutColor = vec4(InPosition.r + 0.5, InPosition.g + 0.5, InPosition.b + 0.5, 1.0);
}
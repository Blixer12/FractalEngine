#version 460

layout(location = 0) in vec3 InPosition;

layout(location = 0) out vec3 OutPosition;

void main()
{
    gl_Position = vec4(InPosition, 1.0);
    OutPosition = InPosition;
}
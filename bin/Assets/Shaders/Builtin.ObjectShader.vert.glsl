#version 460

layout(location = 0) in vec3 InPosition;

layout(set = 0, binding = 0) uniform GlobalUniformObject
{
    mat4 Projection;
    mat4 View;
} GlobalUBO;

layout(push_constant) uniform PushConstants
{   // Guaranteed 128 but i am defaulting to 256, checks in device creation.
    mat4 Model; // 64 Bytes
} PushConstant;


void main()
{
    gl_Position = GlobalUBO.Projection * GlobalUBO.View * PushConstant.Model * vec4(InPosition, 1.0);
}
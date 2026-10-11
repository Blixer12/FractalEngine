#version 460

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec2 TextureCoordinates;

layout(set = 0, binding = 0) uniform GlobalUniformObject
{
    mat4 Projection;
    mat4 View;
} GlobalUBO;

layout(push_constant) uniform PushConstants
{   // Guaranteed 128 but i am defaulting to 256, checks in device creation.
    mat4 Model; // 64 Bytes
} PushConstant;

// layout(location = 0) out int Mode;

layout(location = 1) out struct DataTransferObject {
    vec2 TextureCoordinates;
} OutDTO;

void main()
{
    OutDTO.TextureCoordinates = vec2(TextureCoordinates.x, 1.0 - TextureCoordinates.y);
    gl_Position = GlobalUBO.Projection * GlobalUBO.View * PushConstant.Model * vec4(InPosition, 1.0, 1.0);
}
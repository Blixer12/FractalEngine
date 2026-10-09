#version 460

layout(location = 0) out vec4 OutColor;

layout(set = 1, binding = 0) uniform LocalUniformObject {
    vec4 DiffuseColor;
} ObjectUBO;

layout(set = 1, binding = 1) uniform sampler2D DiffuseSampler;

layout(location = 1) in struct DataTransferObject {
    vec2 TextureCoordinates;
} DTO;

void main()
{
    OutColor = ObjectUBO.DiffuseColor * texture(DiffuseSampler, DTO.TextureCoordinates);
}
#version 450

const vec2 OFFSETS[6]=vec2[](
    vec2(-1.,-1.),
    vec2(-1.,1.),
    vec2(1.,-1.),
    vec2(1.,-1.),
    vec2(-1.,1.),
    vec2(1.,1.)
);

layout(location=0)out vec2 FragOffset;

layout(set=0,binding=0)uniform GlobalUBO
{
    mat4 Projection;
    mat4 View;
    
    vec4 AmbientLightColor;// The 4th Component is Intensity
    vec4 LightPosition;// The 4th Component is Radius
    vec4 LightColor;// The 4th Component is Intensity
}UBO;

const float LightRadius=.1;

void main()
{
    FragOffset=OFFSETS[gl_VertexIndex];
    vec3 CameraRight=vec3(UBO.View[0][0],UBO.View[0][1],UBO.View[0][0]);
    vec3 CameraUp=vec3(UBO.View[0][1],UBO.View[1][1],UBO.View[2][1]);
    
    vec3 WorldPosition=UBO.LightPosition.xyz+LightRadius*FragOffset.x*CameraRight+LightRadius*FragOffset.y*CameraUp;
    
    gl_Position=UBO.Projection*UBO.View*vec4(WorldPosition,1.);
}
#version 450

vec2 Position[3]=vec2[]
(
    vec2(-.5,.5),
    vec2(.5,.5),
    vec2(0,-.5)
);

void main()
{
    gl_Position=vec4(Position[gl_VertexIndex],0,1);
}
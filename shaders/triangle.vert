#version 450

layout (location  = 0) out vec3 outColour;
layout (location = 0) in vec3 positions;

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 1.0f);  

    outColour = colors[gl_VertexIndex];
}
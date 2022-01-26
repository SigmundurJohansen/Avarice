#version 450

layout (location  = 0) out vec3 outColour;

void main()
{
    // const array of position for the triangle
    const vec3 positions[3] = vec3[3](
        vec3(1.f, 1.f, 0.f), //bottom right  
        vec3(-1.f, 1.f, 0.f), // bottom left
        vec3(0.f, -1.f, 0.f)  // middle top
    );

    const vec3 colors[3] = vec3[3](
        vec3(1.f, 0.f, 0.f), //bottom right  
        vec3(0.f, 1.f, 0.f), // bottom left
        vec3(0.f, 0.f, 1.f)  // middle top
    );

    gl_Position = vec4(positions[gl_VertexIndex], 1.0f);  

    outColour = colors[gl_VertexIndex];
}
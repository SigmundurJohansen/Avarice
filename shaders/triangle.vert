#version 450

void main()
{
    // const array of position for the triangle
    const vec3 positions[3] = vec3[3](
        vec3(1.f, 1.f, 0.f), //bottom right  
        vec3(-1.f, 1.f, 0.f), // bottom left
        vec3(0.f, -1.f, 0.f)  // middle top
    );

    gl_Position = vec4(positions[gl_VertexIndex], 1.0f);  
}
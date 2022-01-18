#version 450

// declare out frag color
layout (location = 0) out vec4 outFragColour;

void main()
{
    outFragColour = vec4(1.f, 0.5f, 0.25f, 1.0f);
}
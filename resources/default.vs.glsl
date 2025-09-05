#version 330 core

layout (location = 0) in vec2 aPos;

layout (std140) uniform matrix
{
    mat4 projection;
};

void main()
{
    gl_Position = projection * vec4(aPos. 0.0f, 1.0f);
}

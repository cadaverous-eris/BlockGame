#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

out vec4 vertColor;

uniform mat4 matrix;

void main() {
   gl_Position = matrix * vec4(pos, 1.0f);
   vertColor = color;
}
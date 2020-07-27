#version 330 core

layout (location = 0) in vec2 posIn;
layout (location = 1) in vec2 texCoordIn;

out vec2 texCoord;

void main() {
    gl_Position = vec4(posIn.x, posIn.y, 0.0, 1.0); 
    texCoord = texCoordIn;
}
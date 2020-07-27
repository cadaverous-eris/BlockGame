#version 330 core

layout (location = 0) in vec3 posIn;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordIn;

out vec3 normal;
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
   mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

   gl_Position = mvpMatrix * vec4(posIn, 1.0f);
   normal = mat3(transpose(inverse(modelMatrix))) * normalIn;
   texCoord = texCoordIn;
}
#version 330 core

layout (location = 0) in vec4 posXIn;
layout (location = 1) in vec4 posYIn;
layout (location = 2) in vec4 posZIn;
layout (location = 3) in vec4 texUIn;
layout (location = 4) in vec4 texVIn;

out GS_IN {
	vec4 position2;
	vec4 position3;
	vec4 position4;
	vec2 texCoord1;
	vec2 texCoord2;
	vec2 texCoord3;
	vec2 texCoord4;
} quadOut;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	gl_Position = vec4(posXIn.x, posYIn.x, posZIn.x, 1.0f);
	quadOut.position2 = vec4(posXIn.y, posYIn.y, posZIn.y, 1.0f);
	quadOut.position3 = vec4(posXIn.z, posYIn.z, posZIn.z, 1.0f);
	quadOut.position4 = vec4(posXIn.w, posYIn.w, posZIn.w, 1.0f);
	quadOut.texCoord1 = vec2(texUIn.x, texVIn.x);
	quadOut.texCoord2 = vec2(texUIn.y, texVIn.y);
	quadOut.texCoord3 = vec2(texUIn.z, texVIn.z);
	quadOut.texCoord4 = vec2(texUIn.w, texVIn.w);
}
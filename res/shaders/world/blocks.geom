#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in GS_IN {
	vec4 position2;
	vec4 position3;
	vec4 position4;
	vec2 texCoord1;
	vec2 texCoord2;
	vec2 texCoord3;
	vec2 texCoord4;
	vec3 color;
} vertexIn[];

flat out vec3 normal;
out vec2 texCoord;
flat out vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec3 calculateNormal(vec4 pos1, vec4 pos2, vec4 pos3) {
	return normalize(cross(pos1.xyz - pos2.xyz, pos1.xyz - pos3.xyz)); // used by glm
	//return normalize(cross(pos3.xyz - pos1.xyz, pos3.xyz - pos2.xyz)); // also works
	//return normalize(cross(pos1.xyz - pos2.xyz, pos3.xyz - pos2.xyz)); // produces normal in wrong direction
}

void main() {
	mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));

	vec4 pos1 = gl_in[0].gl_Position;
	vec4 pos2 = vertexIn[0].position2;
	vec4 pos3 = vertexIn[0].position3;
	vec4 pos4 = vertexIn[0].position4;

	vec3 normal1 = normalize(normalMatrix * calculateNormal(pos1, pos2, pos3)); // normal vector of tri 1
	vec3 normal2 = normalize(normalMatrix * calculateNormal(pos3, pos2, pos4)); // normal vector of tri 2

	vec2 uv1 = vertexIn[0].texCoord1;
	vec2 uv2 = vertexIn[0].texCoord2;
	vec2 uv3 = vertexIn[0].texCoord3;
	vec2 uv4 = vertexIn[0].texCoord4;

	// translates verts by their normal vector
	//pos1 = (projectionMatrix * viewMatrix * ((modelMatrix * pos1) + vec4(0.25 * normal1, 0.0)));
	//pos2 = (projectionMatrix * viewMatrix * ((modelMatrix * pos2) + vec4(0.25 * normal2, 0.0)));
	//pos3 = (projectionMatrix * viewMatrix * ((modelMatrix * pos3) + vec4(0.25 * normal1, 0.0)));
	//pos4 = (projectionMatrix * viewMatrix * ((modelMatrix * pos4) + vec4(0.25 * normal2, 0.0)));
	pos1 = mvpMatrix * pos1;
	pos2 = mvpMatrix * pos2;
	pos3 = mvpMatrix * pos3;
	pos4 = mvpMatrix * pos4;

	// set the color of all output vertices
	color = vertexIn[0].color;

	// vertex 1
	gl_Position = pos1;
	normal = normal1;
	texCoord = uv1;
	EmitVertex();
	// vertex 2
	gl_Position = pos2;
	normal = normal2;
	texCoord = uv2;
	EmitVertex();
	// vertex 3
	gl_Position = pos3;
	normal = normal1;
	texCoord = uv3;
	EmitVertex();
	// vertex 4
	gl_Position = pos4;
	normal = normal2;
	texCoord = uv4;
	EmitVertex();

	EndPrimitive();

	//// vertex 1
	//gl_Position = pos3;
	//normal = normal1 * -1;
	//texCoord = uv3;
	//EmitVertex();
	//// vertex 2
	//gl_Position = pos4;
	//normal = normal2 * -1;
	//texCoord = uv4;
	//EmitVertex();
	//// vertex 3
	//gl_Position = pos1;
	//normal = normal1 * -1;
	//texCoord = uv1;
	//EmitVertex();
	//// vertex 4
	//gl_Position = pos2;
	//normal = normal2 * -1;
	//texCoord = uv2;
	//EmitVertex();
	//
	//EndPrimitive();
}

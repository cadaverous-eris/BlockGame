#version 330 core

flat in vec3 normal;
in vec2 texCoord;
flat in vec3 color;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	vec4 objectColor = texture(textureSampler, texCoord) * vec4(color, 1.0f);
	if (objectColor.a < 0.99) discard;

	vec3 lightColor = vec3(1.0, 0.8625, 0.98);//vec3(1.0f, 1.0f, 1.0f);
	//vec3 lightDir = -normalize(vec3(0.25f, -1.0f, -0.5f));
	vec3 lightDir = -normalize(vec3(0.7f, -0.9f, 0.0f));

	float ambientStrength = 0.4f;

	vec3 ambient = ambientStrength * lightColor;
	float cosTheta = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = cosTheta * lightColor;

	FragColor = vec4(ambient + diffuse, 1.0f) * objectColor;
	//FragColor = vec4(ambient + (diffuse * (1 - ambientStrength)), 1.0f) * objectColor;
}
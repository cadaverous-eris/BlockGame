#version 330 core

in vec2 vertUV;
in vec4 vertColor;

out vec4 FragColor;

uniform sampler2D textureSampler;

void main() {
	vec4 texColor = texture(textureSampler, vertUV);
	vec4 fragColor = texColor * vertColor;

	if (fragColor.a < 0.025) discard;

	FragColor = fragColor;
}
#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D accumTexSampler;
uniform sampler2D revealageTexSampler;
uniform sampler2D colorTexSampler;

float componentMax(vec4 v) {
	return max(v.r, max(v.g, max(v.b, v.a)));
}

void main() {
	float revealage = texelFetch(revealageTexSampler, ivec2(gl_FragCoord.xy), 0).r;
	if (revealage == 1.0)
		discard;

	vec4 accum = texelFetch(accumTexSampler, ivec2(gl_FragCoord.xy), 0);
	if (isinf(componentMax(abs(accum))))
        accum.rgb = vec3(accum.a);

	FragColor = vec4(accum.rgb / clamp(accum.a, 1e-4, 5e4), revealage);
}
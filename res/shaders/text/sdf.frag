#version 330 core

//flat in vec3 normal;
in vec2 texCoord;
in vec4 fontColor;
in vec4 outlineColor;
in float outlineSize;
in float outlineSpread;
in float softness;

out vec4 FragColor;
//layout (depth_greater) out float gl_FragDepth;

uniform float edgeVal;
uniform float padding;
uniform float pixelDistScale;
uniform sampler2D textureSampler;

float sample(vec2 uv) {
	float dist = texture(textureSampler, uv).r;
	// Antialias
	float dscale = 0.354; // half of 1/sqrt2
	float friends = 0.5;  // scale value to apply to neighbours
	vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
	vec4 box = vec4(uv - duv, uv + duv);
	float d = texture(textureSampler, box.xy).r +
			  texture(textureSampler, box.zw).r +
			  texture(textureSampler, box.xw).r +
			  texture(textureSampler, box.zy).r;
	float sum = 4.0; // 4 neighbouring samples
	dist = (dist + (friends * d)) / (1.0 + (sum * friends));
	return dist;
}

vec4 getColor(vec2 uv, vec4 color) {
	float t = sample(uv);
	float edgeFactor = smoothstep(edgeVal - softness, edgeVal, t);
	gl_FragDepth = gl_FragCoord.z;
	return vec4(color.rgb, color.a * edgeFactor);
}
vec4 getColor(vec2 uv, vec4 color, vec4 outlineColor) {
	float outlineSoftness = max(softness, outlineSpread);
	float t = sample(uv);
	float outlineFactor = 1.0 - smoothstep(edgeVal - softness, edgeVal, t);
	gl_FragDepth = gl_FragCoord.z + (outlineFactor * 0.001); // TODO: properly scale depth increase for non-linear gl_FragDepth value
	vec4 c = mix(color, outlineColor, outlineFactor);
	float outlineVal = edgeVal - outlineSize;
	float edgeFactor = smoothstep(outlineVal - outlineSoftness, outlineVal, t);
	return vec4(c.rgb, c.a * edgeFactor);
}

const float eps = 0.0005;
void main() {
	bool hasOutline = (outlineSize + outlineSpread > eps) && (outlineColor.a > eps);
	FragColor = hasOutline ? getColor(texCoord, fontColor, outlineColor) : getColor(texCoord, fontColor);
}
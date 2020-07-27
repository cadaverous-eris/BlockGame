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
	vec4 color;
	vec4 outlineColor;
	float outlineSize;
	float outlineSpread;
} vertexIn[];

//flat out vec3 normal;
out vec2 texCoord;
out vec4 fontColor;
out vec4 outlineColor;
out float outlineSize;
out float outlineSpread;
out float softness;

uniform vec2 framebufferSize;
uniform float edgeVal;
uniform float padding;
uniform float pixelDistScale;
uniform sampler2D textureSampler;
uniform mat4 mvpMatrix;

vec3 calculateNormal(vec4 pos1, vec4 pos2, vec4 pos3) {
	return normalize(cross(pos3.xyz - pos1.xyz, pos3.xyz - pos2.xyz));
}

void main() {
	vec4 pos1 = gl_in[0].gl_Position;
	vec4 pos2 = vertexIn[0].position2;
	vec4 pos3 = vertexIn[0].position3;
	vec4 pos4 = vertexIn[0].position4;
	vec2 uv1 = vertexIn[0].texCoord1;
	vec2 uv2 = vertexIn[0].texCoord2;
	vec2 uv3 = vertexIn[0].texCoord3;
	vec2 uv4 = vertexIn[0].texCoord4;
	vec4 color = vertexIn[0].color;
	vec4 olColor = vertexIn[0].outlineColor;
	float olSize = vertexIn[0].outlineSize * pixelDistScale / 255.0;
	float olSpread = vertexIn[0].outlineSpread * pixelDistScale / 255.0;

	vec4 spos1 = mvpMatrix * pos1;
	vec4 spos2 = mvpMatrix * pos2;
	vec4 spos3 = mvpMatrix * pos3;
	vec4 spos4 = mvpMatrix * pos4;

	vec2 glyphTexDim = abs((uv4 * textureSize(textureSampler, 0)) - (uv1 * textureSize(textureSampler, 0)));
	float glyphTexSize = (glyphTexDim.x + glyphTexDim.y) / 2.0;
	vec2 minQuadSPos = min(min(min(spos1.xy, spos2.xy), spos3.xy), spos4.xy) * framebufferSize / 2.0;
	vec2 maxQuadSPos = max(max(max(spos1.xy, spos2.xy), spos3.xy), spos4.xy) * framebufferSize / 2.0;
	vec2 renderDim = maxQuadSPos - minQuadSPos;
	float glyphRenderSize = (renderDim.x + renderDim.y) / 2.0;
	float glyphScale = (glyphRenderSize / glyphTexSize);

	// scale edge softness based on glyph size
	float edgeSoftness = ((0.65 + (0.7 * smoothstep(0.35, 1.65, glyphScale))) / glyphScale) * (pixelDistScale / 255.0);
	// scale outline based on glyph size
	if (glyphScale < 1.3) {
		float olScale = (0.0 + (1.0 / glyphScale)) / 1.0;
		float olScaledDist = min(olScale * (olSize + olSpread), (padding - edgeSoftness) * pixelDistScale / 255.0);
		olSize = olScaledDist * (olSize / (olSize + olSpread));
		olSpread = olScaledDist * (olSpread / (olSize + olSpread)) + (edgeSoftness / olScale);
	}

	// vertex 1
	gl_Position = spos1;
	texCoord = uv1;
	fontColor = color;
	outlineColor = olColor;
	outlineSize = olSize;
	outlineSpread = olSpread;
	softness = edgeSoftness;
	EmitVertex();
	// vertex 2
	gl_Position = spos2;
	texCoord = uv2;
	fontColor = color;
	outlineColor = olColor;
	outlineSize = olSize;
	outlineSpread = olSpread;
	softness = edgeSoftness;
	EmitVertex();
	// vertex 3
	gl_Position = spos3;
	texCoord = uv3;
	fontColor = color;
	outlineColor = olColor;
	outlineSize = olSize;
	outlineSpread = olSpread;
	softness = edgeSoftness;
	EmitVertex();
	// vertex 4
	gl_Position = spos4;
	texCoord = uv4;
	fontColor = color;
	outlineColor = olColor;
	outlineSize = olSize;
	outlineSpread = olSpread;
	softness = edgeSoftness;
	EmitVertex();

	EndPrimitive();
}
#version 330 core

flat in vec3 normal;
in vec2 texCoord;
flat in vec3 color;

layout(location = 0) out vec4 accum;

uniform sampler2D textureSampler;
uniform sampler2D opaqueColorTexSampler;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void writePixel(vec4 premultipliedReflect, vec4 transmit, float csZ) {
	premultipliedReflect.a *= 1.0 - (transmit.r + transmit.g + transmit.b) * (1.0 / 3.0);

	// intermediate terms to be cubed
	float tmp = (premultipliedReflect.a * 8.0 + 0.01) * (-gl_FragCoord.z * 0.95 + 1.0);
	//tmp /= sqrt(abs(csZ)); // can be enabled if a lot of the scene is close to the far plane

	float w = clamp(tmp * tmp * tmp * 1e3, 1e-2, 3e2);

	accum = premultipliedReflect * w;
}

void main() {
	vec4 objectColor = texture(textureSampler, texCoord) * vec4(color, 1.0f);//vec4(1.0f, 0.5f, 0.2f, 1.0f);
	//if (objectColor.a >= 0.99) discard;

	vec3 lightColor = vec3(1.0, 0.8625, 0.98);//vec3(1.0f, 1.0f, 1.0f);
	//vec3 lightDir = -normalize(vec3(0.25f, -1.0f, -0.5f));
	vec3 lightDir = -normalize(vec3(0.7f, -0.9f, 0.0f));

	float ambientStrength = 0.4f;

	vec3 ambient = ambientStrength * lightColor;
	float cosTheta = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = cosTheta * lightColor;

	vec4 fragColor = vec4(ambient + diffuse, 1.0f) * objectColor;
	//vec4 fragColor = vec4(ambient + (diffuse * (1.0f - ambientStrength)), 1.0f) * objectColor;

	vec4 fragColorPremult = vec4(fragColor.rgb * fragColor.a, fragColor.a);
	vec4 transmitColor = vec4(0.0, 0.0, 0.0, 1.0);//texelFetch(opaqueColorTexSampler, ivec2(gl_FragCoord.xy), 0);
	float csZ = gl_FragCoord.z;
	writePixel(fragColorPremult, transmitColor, csZ);
}
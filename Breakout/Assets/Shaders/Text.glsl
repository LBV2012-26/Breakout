#version 460 core

layout(location = 0) out vec4 FragColor;
					 in  vec2 TexCoords;

uniform sampler2D TextTexture;
uniform vec3      TextColor;

void main() {
	vec4 Sampled = vec4(1.0, 1.0, 1.0, texture(TextTexture, TexCoords).r);
	FragColor    = vec4(TextColor, 1.0f) * Sampled;
}

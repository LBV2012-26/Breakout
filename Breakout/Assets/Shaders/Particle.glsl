#version 460 core

layout(location = 0) out vec4 FragColor;
					 in  vec2 TexCoords;

uniform sampler2D ParticleTex;
uniform vec4      ParticleBright;
uniform bool      bBurned;

void main() {
	vec4 TexColor = vec4(1.0f);
	if (bBurned) {
		TexColor = texture(ParticleTex, TexCoords).bgra;
	} else {
		TexColor = texture(ParticleTex, TexCoords);
	}

	FragColor = ParticleBright * TexColor;
}

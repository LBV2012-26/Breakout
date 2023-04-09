#version 460 core

layout(location = 0) out vec4 FragColor;
					 in  vec2 TexCoords;

uniform sampler2D TexColorBuffer;
uniform bool      bChaos;
uniform bool      bConfuse;
uniform bool      bShake;

const float kOffset = 1.0 / 300.0;

void main() {
	FragColor = vec4(0.0);

	vec2 Offsets[9] = {
		vec2(-kOffset,  kOffset),
        vec2( 0.0,      kOffset),
        vec2( kOffset,  kOffset),
        vec2(-kOffset,  0.0),
        vec2( 0.0,      0.0),
        vec2( kOffset,  0.0),
        vec2(-kOffset, -kOffset),
        vec2( 0.0,     -kOffset),
        vec2( kOffset, -kOffset)
	};

    int EdgeKernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };

    float BlurKernel[9] = {
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
    };

	vec3 SampledTex[9];

	if (bChaos || bShake) {
		for (int i = 0; i != 9; ++i) {
			SampledTex[i] = vec3(texture(TexColorBuffer, TexCoords.st + Offsets[i]));
		}
	}

	if (bChaos) {
		for (int i = 0; i != 9; ++i) {
			FragColor += vec4(SampledTex[i] * EdgeKernel[i], 1.0);
		}
	} else if (bConfuse) {
		FragColor = vec4(1.0 - texture(TexColorBuffer, TexCoords).rgb, 1.0);
	} else if (bShake) {
		for (int i = 0; i != 9; ++i) {
			FragColor += vec4(SampledTex[i] * BlurKernel[i], 1.0);
		}
	} else {
		FragColor = texture(TexColorBuffer, TexCoords);
	}
}

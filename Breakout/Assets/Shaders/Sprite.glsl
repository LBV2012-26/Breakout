#version 460 core

layout(location = 0) out vec4 FragColor;
					 in  vec2 TexCoords;

uniform sampler2D SpriteTex;
uniform vec3      SpriteColor;

void main() {
	FragColor = vec4(SpriteColor, 1.0) * texture(SpriteTex, TexCoords);
}

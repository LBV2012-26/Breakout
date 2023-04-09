#version 460 core

layout(location = 0) in  vec4 Vertex;
					 out vec2 TexCoords;

#if defined(PARTICLE)
uniform mat4x4 Projection;
uniform vec2   Offset;
#elif defined(TEXT)
uniform mat4x4 Projection;
#else
uniform mat4x4 Model;
uniform mat4x4 Projection;
#endif

void main() {
	TexCoords   = Vertex.zw;
#if defined(PARTICLE)
	float Scale = 20.0;
	gl_Position = Projection * vec4((Vertex.xy * Scale) + Offset, 0.0, 1.0);
#elif defined(TEXT)
	gl_Position = Projection * vec4(Vertex.xy, 0.0, 1.0);
#else
	gl_Position = Projection * Model * vec4(Vertex.xy, 0.0, 1.0);
#endif
}

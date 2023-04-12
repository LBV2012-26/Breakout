#version 460 core

layout(location = 0) in  vec4 Vertex;
                     out vec2 TexCoords;

uniform bool  bChaos;
uniform bool  bConfuse;
uniform bool  bShake;
uniform float Time;

void main() {
    gl_Position = vec4(Vertex.xy, 0.0, 1.0);
    vec2 InputTexCoords = Vertex.zw;

    if (bChaos) {
        float Strength = 0.3;
        vec2  Position = vec2(InputTexCoords.x + sin(Time) * Strength, InputTexCoords.y + cos(Time) * Strength);
        TexCoords = Position;
    } else if (bConfuse) {
        TexCoords = vec2(1.0 - InputTexCoords.xy);
    } else {
        TexCoords = InputTexCoords;
    }

    if (bChaos && bConfuse) {
        float Strength = 0.3;
        vec2  Position = vec2(1.0 - (InputTexCoords.x + sin(Time) * Strength), 1.0 - (InputTexCoords.y + cos(Time) * Strength));
        TexCoords = Position;
    }

    if (bShake) {
        float Strength = 0.01;
        gl_Position.x += cos(Time * 10.0) * Strength;
        gl_Position.y += cos(Time * 15.0) * Strength;
    }
}

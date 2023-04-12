#pragma once
#ifndef GAME_RENDERER_SPRITE_H_
#define GAME_RENDERER_SPRITE_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../AssetLoader/Shader.h"
#include "../../AssetLoader/Texture.h"

class SpriteRenderer {
public:
    SpriteRenderer() = default;
    SpriteRenderer(const Shader* SpriteShader);
    ~SpriteRenderer();

    GLvoid Draw(const Texture2D* Texture,
                const glm::vec2& Position,
                const glm::vec2& Size     = glm::vec2(1.0f),
                GLfloat          Rotation = 0.0f,
                const glm::vec3& Color    = glm::vec3(1.0f)) const;

private:
    const Shader* _SpriteShader;
    GLuint        _VertexArray;
};

#endif // !GAME_RENDERER_SPRITE_H_

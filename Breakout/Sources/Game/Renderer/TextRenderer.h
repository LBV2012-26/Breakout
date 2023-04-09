#pragma once
#ifndef GAME_RENDERER_TEXTRENDERER_H_
#define GAME_RENDERER_TEXTRENDERER_H_

#include <map>
#include <memory>
#include <string>

#include <freetype/freetype.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../AssetLoader/Shader.h"

class TextRenderer {
private:
    struct Character {
        const TextureCharacter* TexData;
        glm::ivec2 Size;
        glm::ivec2 Bearings;
        GLint      Advance;
    };

public:
    TextRenderer() = default;
    TextRenderer(const Shader* TextShader, const std::string& FontFilename, GLsizei Width, GLsizei Height, FT_UInt FontSize);
    ~TextRenderer();

    GLvoid Draw(const std::string& Text, const glm::vec2& Position, GLfloat Scale, const glm::vec3& Color = glm::vec3(1.0f));

private:
    GLvoid LoadFont(const std::string& FontFilepath, FT_UInt FontSize);

private:
    const Shader*               _TextShader;
    std::map<GLchar, Character> _Characters;

    GLuint                      _VertexArray;
    GLuint                      _VertexBuffer;
};

#endif // !GAME_RENDERER_TEXTRENDERER_H_

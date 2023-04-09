#pragma once
#ifndef GAME_RENDERER_TEXTRENDERER_H_
#define GAME_RENDERER_TEXTRENDERER_H_

#include <glad/glad.h>
#include "../../AssetLoader/Shader.h"

class TextRenderer {
public:
    TextRenderer() = default;
    GLvoid Draw(const Shader* TextShader);

};

#endif // !GAME_RENDERER_TEXTRENDERER_H_

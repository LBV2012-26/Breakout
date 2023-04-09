#pragma once
#ifndef GAME_RENDERER_POSTPROCESSOR_H_
#define GAME_RENDERER_POSTPROCESSOR_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../AssetLoader/Shader.h"
#include "../../AssetLoader/Texture.h"
#include "Sprite.h"

enum class Effects {
    kChaos, kConfuse, kShake
};

class PostProcessor {
public:
    PostProcessor() = default;
    PostProcessor(const Shader* PostShader, GLsizei Width, GLsizei Height, GLsizei MultiSamples);
    ~PostProcessor();

    GLvoid Render(GLfloat Time);
    GLvoid SetEffect(Effects Effect, GLboolean bValue);

public:
    GLvoid BeginRender() {
        glBindFramebuffer(GL_FRAMEBUFFER, _MultiSampledFramebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    GLvoid EndRender() {
        glBlitNamedFramebuffer(_MultiSampledFramebuffer, _IntermediateFramebuffer,
                               0, 0, _Width, _Height, 0, 0, _Width, _Height,
                               GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    const Shader*            _PostShader;
    const TextureAttachment* _TexColorBuffer;
    GLuint                   _MultiSampledFramebuffer;
    GLuint                   _IntermediateFramebuffer;
    GLuint                   _Renderbuffer;
    GLuint                   _VertexArray;
    GLsizei                  _MultiSamples;

    GLsizei                  _Width;
    GLsizei                  _Height;

    GLboolean                _bChaos;
    GLboolean                _bConfuse;
    GLboolean                _bShake;
};

#endif // !GAME_RENDERER_POSTPROCESSOR_H_

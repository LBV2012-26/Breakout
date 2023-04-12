#include "PostProcessor.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "../../Constants.h"

#ifdef NDEBUG
#include <Windows.h>
#endif // NDEBUG

PostProcessor::PostProcessor(const Shader* PostShader, GLsizei Width, GLsizei Height, GLsizei MultiSamples) :
    _PostShader(PostShader), _Width(Width), _Height(Height), _MultiSamples(MultiSamples) {
    glCreateFramebuffers(1, &_MultiSampledFramebuffer);
    glCreateFramebuffers(1, &_IntermediateFramebuffer);
    glCreateRenderbuffers(1, &_Renderbuffer);
    glNamedRenderbufferStorageMultisample(_Renderbuffer, _MultiSamples, GL_RGB8, _Width, _Height);
    glNamedFramebufferRenderbuffer(_MultiSampledFramebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _Renderbuffer);

    if (glCheckNamedFramebufferStatus(_MultiSampledFramebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef _DEBUG
        std::cerr << "Error: Failed to initialize multi-sample framebuffer." << std::endl;
        std::system("pause");
#else
        MessageBoxA(nullptr, "Failed to initialize multi-sample framebuffer.", "Framebuffer Incomplete", MB_ICONERROR);
#endif
        std::exit(EXIT_FAILURE);
    }

    _TexColorBuffer = new TextureAttachment(_Width, _Height, GL_RGB8, GL_COLOR_ATTACHMENT0, _IntermediateFramebuffer);

    if (glCheckNamedFramebufferStatus(_IntermediateFramebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef _DEBUG
        std::cerr << "Error: Failed to initialize intermediate framebuffer." << std::endl;
        std::system("pause");
#else
        MessageBoxA(nullptr, "Failed to initialize intermediate framebuffer.", "Framebuffer Incomplete", MB_ICONERROR);
#endif
        std::exit(EXIT_FAILURE);
    }

    _TexColorBuffer->BindTextureUnit(_PostShader, "TexColorBuffer", kTexColorBufferTexUnit);

#include "Vertices.inc"

    GLuint VertexBuffer = 0;

    glCreateVertexArrays(1, &_VertexArray);
    glCreateBuffers(1, &VertexBuffer);

    glNamedBufferData(VertexBuffer, QuadVertices.size() * sizeof(GLfloat), QuadVertices.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(_VertexArray, 0, VertexBuffer, 0, 4 * sizeof(GLfloat));

    glEnableVertexArrayAttrib(_VertexArray, 0);
    glVertexArrayAttribFormat(_VertexArray, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VertexArray, 0, 0);

    glDeleteBuffers(1, &VertexBuffer);
}

PostProcessor::~PostProcessor() {
    glDeleteVertexArrays(1, &_VertexArray);
    glDeleteFramebuffers(1, &_MultiSampledFramebuffer);
    glDeleteFramebuffers(1, &_IntermediateFramebuffer);
    glDeleteRenderbuffers(1, &_Renderbuffer);

    // The memory will release after AssetManager execute destruction function.
    // ------------------------------------------------------------------------
    // if (_PostShader) {
    //     delete _PostShader;
    //     _PostShader = nullptr;
    // }

    // if (_TexColorBuffer) {
    //     delete _TexColorBuffer;
    //     _TexColorBuffer = nullptr;
    // }
}

GLvoid PostProcessor::Render(GLfloat Time) {
    _PostShader->UseProgram();
    _PostShader->SetUniform1f("Time",     Time);
    _PostShader->SetUniform1i("bChaos",   _bChaos);
    _PostShader->SetUniform1i("bConfuse", _bConfuse);
    _PostShader->SetUniform1i("bShake",   _bShake);

    _TexColorBuffer->BindTextureUnit(_PostShader, "TexColorBuffer", kTexColorBufferTexUnit);
    glBindVertexArray(_VertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

GLvoid PostProcessor::SetEffectState(Effects Effect, GLboolean bValue) {
    switch (Effect) {
    case Effects::kChaos:
        _bChaos = bValue;
        break;
    case Effects::kConfuse:
        _bConfuse = bValue;
        break;
    case Effects::kShake:
        _bShake = bValue;
        break;
    }
}

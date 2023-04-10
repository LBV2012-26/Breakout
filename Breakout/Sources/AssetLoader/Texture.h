#pragma once
#ifndef ASSETLOADER_TEXTURE_H_
#define ASSETLOADER_TEXTURE_H_

#include <string>

#include <freetype/freetype.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

class Texture2D {
public:
    Texture2D() = default;
    Texture2D(const std::string& ImageFilename, GLboolean bAutoFillFilepath = GL_TRUE);
    ~Texture2D();

    GLvoid BindTextureUnit(const Shader* ActivatedShader, const std::string& UniformName, GLuint Unit) const;

public:
    GLuint GetTexture() const {
        return _Texture;
    }

private:
    GLuint _Texture{};
};

class TextureAttachment {
public:
    TextureAttachment() = default;
    TextureAttachment(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer);
    TextureAttachment(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLsizei Samples,
                      GLenum Attachment, GLboolean bFixedSampleLocations, GLuint Framebuffer);
    ~TextureAttachment();

    GLvoid BindTextureUnit(const Shader* ActivatedShader, const std::string& UniformName, GLuint Unit) const;

public:
    GLuint GetTexture() const {
        return _Texture;
    }

private:
    GLuint _Texture{};
};

class TextureCharacter {
public:
    TextureCharacter() = default;
    TextureCharacter(const FT_Face& Face);
    ~TextureCharacter();

    GLvoid BindTextureUnit(const Shader* ActivatedShader, const std::string& UniformName, GLuint Unit) const;

public:
    GLuint GetTexture() const {
        return _Texture;
    }

private:
    GLuint _Texture{};
};

#endif // !ASSETLOADER_TEXTURE_H_

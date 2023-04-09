#include "Texture.h"

#include <cstdlib>
#include <format>
#include <iostream>

#include <stb_image.h>

#include "GetAssetFilepath.h"

Texture2D::Texture2D(const std::string& ImageFilename, GLboolean bAutoFillFilepath) {
    GLint ImageWidth    = 0;
    GLint ImageHeight   = 0;
    GLint ImageChannels = 0;

    std::string ImageFilepath;
    if (bAutoFillFilepath) {
        ImageFilepath = GetAssetFilepath(AssetType::kTexture, ImageFilename);
    } else {
        ImageFilepath = ImageFilename;
    }

    // stbi_set_flip_vertically_on_load(GL_TRUE);
    GLubyte* ImageData = stbi_load(ImageFilepath.c_str(), &ImageWidth, &ImageHeight, &ImageChannels, 0);

    if (!ImageData) {
        std::cerr << std::format("Fatal error: Can not open image file: \"{}\": No such fire or directory.", ImageFilepath)
                  << std::endl;
        std::system("pause");
        std::exit(EXIT_FAILURE);
    }

    GLenum ImageFormat    = 0;
    GLenum InternalFormat = 0;
    GLenum TextureWarpS   = 0;
    GLenum TextureWarpT   = 0;

    switch (ImageChannels) {
    case 3: {
        ImageFormat    = GL_RGB;
        InternalFormat = GL_RGB16;
        TextureWarpS   = GL_REPEAT;
        TextureWarpT   = GL_REPEAT;
        break;
    }
    case 4: {
        ImageFormat    = GL_RGBA;
        InternalFormat = GL_RGBA16;
        TextureWarpS   = GL_CLAMP_TO_EDGE;
        TextureWarpT   = GL_CLAMP_TO_EDGE;
        break;
    }
    default: {
        break;
    }
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);

    glTextureStorage2D(_Texture, 1, InternalFormat, ImageWidth, ImageHeight);
    glTextureSubImage2D(_Texture, 0, 0, 0, ImageWidth, ImageHeight, ImageFormat, GL_UNSIGNED_BYTE, ImageData);
    glGenerateTextureMipmap(_Texture);

    glTextureParameteri(_Texture, GL_TEXTURE_WRAP_S, TextureWarpS);
    glTextureParameteri(_Texture, GL_TEXTURE_WRAP_T, TextureWarpT);
    glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(ImageData);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &_Texture);
}

GLvoid Texture2D::BindTextureUnit(const Shader* ActivatedShader, const std::string& UniformName) const {
    glBindTextureUnit(_Texture - 1, _Texture);
    ActivatedShader->SetUniform1i(UniformName, static_cast<GLint>(_Texture - 1));
}

TextureAttachment::TextureAttachment(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLenum Attachment, GLuint Framebuffer) {
    glCreateTextures(GL_TEXTURE_2D, 1, &_Texture);
    glTextureParameteri(_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(_Texture, 1, InternalFormat, Width, Height);
    glNamedFramebufferTexture(Framebuffer, Attachment, _Texture, 0);
}

TextureAttachment::TextureAttachment(GLsizei Width, GLsizei Height, GLenum InternalFormat, GLsizei Samples,
                                     GLenum Attachment, GLboolean bFixedSampleLocations, GLuint Framebuffer) {
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &_Texture);
    glTextureStorage2DMultisample(_Texture, Samples, InternalFormat, Width, Height, bFixedSampleLocations);
    glNamedFramebufferTexture(Framebuffer, Attachment, _Texture, 0);
}

TextureAttachment::~TextureAttachment() {
    glDeleteTextures(1, &_Texture);
}

GLvoid TextureAttachment::BindTextureUnit(const Shader* ActivatedShader, const std::string& UniformName) const {
    glBindTextureUnit(_Texture - 1, _Texture);
    ActivatedShader->SetUniform1i(UniformName, static_cast<GLint>(_Texture - 1));
}

// stb_image implementation
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

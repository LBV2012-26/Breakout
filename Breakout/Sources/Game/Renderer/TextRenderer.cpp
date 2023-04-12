#include "TextRenderer.h"

#include <cstdlib>
#include <format>
#include <iostream>
#include <utility>
#include <vector>

#include "../../AssetLoader/GetAssetFilepath.h"
#include "../../AssetLoader/Texture.h"
#include "../../Constants.h"

#ifdef NDEBUG
#include <Windows.h>
#endif // NDEBUG

TextRenderer::TextRenderer(const Shader* TextShader, const std::string& FontFilename, GLsizei Width, GLsizei Height, FT_UInt FontSize) :
    _TextShader(TextShader) {
    glm::mat4x4 Projection = glm::ortho(0.0f, static_cast<GLfloat>(Width), static_cast<GLfloat>(Height), 0.0f);
    _TextShader->UseProgram();
    _TextShader->SetUniformMatrix4fv("Projection", Projection);
    _TextShader->SetUniform1i("TextTexture", 0);

    glCreateVertexArrays(1, &_VertexArray);
    glCreateBuffers(1, &_VertexBuffer);

    glNamedBufferData(_VertexBuffer, 24ULL * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
    glVertexArrayVertexBuffer(_VertexArray, 0, _VertexBuffer, 0, 4 * sizeof(GLfloat));

    glEnableVertexArrayAttrib(_VertexArray, 0);
    glVertexArrayAttribFormat(_VertexArray, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VertexArray, 0, 0);

    LoadFont(GetAssetFilepath(AssetType::kFont, FontFilename), FontSize);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &_VertexArray);
    glDeleteBuffers(1, &_VertexBuffer);

    // if (_TextShader) {
    //     delete _TextShader;
    //     _TextShader = nullptr;
    // }
}

GLvoid TextRenderer::Draw(const std::string& Text, GLfloat x, GLfloat y, GLfloat Scale, const glm::vec3& Color) {
    _TextShader->UseProgram();
    _TextShader->SetUniform3fv("TextColor", Color);
    glBindVertexArray(_VertexArray);

    for (const auto CharCode : Text) {
        Character Char      = _Characters[CharCode];
        GLfloat   PositionX = x + Char.Bearings.x * Scale;
        GLfloat   PositionY = y + (_Characters['H'].Bearings.y - Char.Bearings.y) * Scale;
        GLfloat   Width     = Char.Size.x * Scale;
        GLfloat   Height    = Char.Size.y * Scale;

        std::vector<GLfloat> Vertices{
            PositionX,         PositionY + Height, 0.0f, 1.0f,
            PositionX + Width, PositionY,          1.0f, 0.0f,
            PositionX,         PositionY,          0.0f, 0.0f,

            PositionX,         PositionY + Height, 0.0f, 1.0f,
            PositionX + Width, PositionY + Height, 1.0f, 1.0f,
            PositionX + Width, PositionY,          1.0f, 0.0f
        };

        Char.TexData->BindTextureUnit(_TextShader, "TexTexture", kSpriteTexUnit);

        glNamedBufferSubData(_VertexBuffer, 0, Vertices.size() * sizeof(GLfloat), Vertices.data());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (Char.Advance >> 6) * Scale;
    }

    glBindVertexArray(0);
}

GLvoid TextRenderer::LoadFont(const std::string& FontFilepath, FT_UInt FontSize) {
    _Characters.clear();

    FT_Library Library;
    if (FT_Init_FreeType(&Library)) {
        std::cout << "Error: Failed to initialize FreeType Library." << std::endl;
        std::system("pause");
        std::exit(EXIT_FAILURE);
    }

    FT_Face Face;
    if (FT_New_Face(Library, FontFilepath.c_str(), 0, &Face)) {
#ifdef _DEBUG
        std::cerr << std::format("Fatal error: Can not open font file \"{}\": No such file or directory.", FontFilepath)
                  << std::endl;
        std::system("pause");
#else
        MessageBoxA(nullptr, std::format("Fatal error: Can not open font file \"{}\": No such file or directory.", FontFilepath).c_str(), "Font Load Error", MB_ICONERROR);
#endif
        std::exit(EXIT_FAILURE);
    }

    FT_Set_Pixel_Sizes(Face, 0, FontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLboolean bAllGLyphLoadSuccess = GL_TRUE;
    for (GLubyte Char = 0; Char != 128; ++Char) {
        if (FT_Load_Char(Face, Char, FT_LOAD_RENDER)) {
            std::cerr << "Error: Failed to load glyph." << std::endl;
            bAllGLyphLoadSuccess = GL_FALSE;
            continue;
        }

        Character CharData{
            new TextureCharacter(Face),
            glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows),
            glm::ivec2(Face->glyph->bitmap_left,  Face->glyph->bitmap_top),
            Face->glyph->advance.x
        };

        _Characters.insert(std::make_pair(Char, CharData));
    }

#ifdef NDEBUG
    if (!bAllGLyphLoadSuccess) {
        MessageBoxA(nullptr, "Warnig: Error generated. Not all glyph loaded.", "Font Load Warning", MB_ICONWARNING);
    }
#endif

    FT_Done_Face(Face);
    FT_Done_FreeType(Library);
}

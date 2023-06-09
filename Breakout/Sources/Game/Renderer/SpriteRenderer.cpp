#include "SpriteRenderer.h"
#include <vector>

#include "../../AssetLoader/AssetManager.h"
#include "../../Constants.h"

SpriteRenderer::SpriteRenderer(const Shader* SpriteShader) : _SpriteShader(SpriteShader), _VertexArray(0) {
    GLuint VertexBuffer = 0;

    glCreateVertexArrays(1, &_VertexArray);
    glCreateBuffers(1, &VertexBuffer);

#include "Vertices.inc"

    glNamedBufferData(VertexBuffer, Vertices.size() * sizeof(GLfloat), Vertices.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(_VertexArray, 0, VertexBuffer, 0, 4 * sizeof(GLfloat));

    glEnableVertexArrayAttrib(_VertexArray, 0);
    glVertexArrayAttribFormat(_VertexArray, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VertexArray, 0, 0);

    glDeleteBuffers(1, &VertexBuffer);
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &_VertexArray);

    // The memory will release after AssetManager execute destruction function.
    // ------------------------------------------------------------------------
    // if (_SpriteShader) {
    //     delete _SpriteShader;
    //     _SpriteShader = nullptr;
    // }
}

GLvoid SpriteRenderer::Draw(const Texture2D* Texture, const glm::vec2& Position, const glm::vec2& Size, GLfloat Rotation, const glm::vec3& Color) const {
    glm::mat4x4 Model(1.0f);

    Model = glm::translate(Model, glm::vec3(Position, 0.0f));
    Model = glm::translate(Model, glm::vec3(Size.x *  0.5f, Size.y *  0.5f, 0.0f));
    Model = glm::rotate   (Model, Rotation, glm::vec3(0.0f,           0.0f, 1.0f));
    Model = glm::translate(Model, glm::vec3(Size.x * -0.5f, Size.y * -0.5f, 0.0f));
    Model = glm::scale    (Model, glm::vec3(Size,     1.0f));

    _SpriteShader->UseProgram();
    _SpriteShader->SetUniformMatrix4fv("Model", Model);
    _SpriteShader->SetUniform3fv("SpriteColor", Color);

    Texture->BindTextureUnit(_SpriteShader, "SpriteTex", kSpriteTexUnit);

    glBindVertexArray(_VertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

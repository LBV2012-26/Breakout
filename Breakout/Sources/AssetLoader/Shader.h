#pragma once
#ifndef ASSETLOADER_SHADER_H_
#define ASSETLOADER_SHADER_H_

#include <set>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
    struct Source {
        std::string Data;
        std::string Filepath;
        GLboolean   bHasInclude;
        GLboolean   bHasMacros;
    };

public:
    Shader() = default;
    Shader(const std::vector<std::string>& SourceFiles, const std::vector<std::string>& Macros = { "NULL" });
    ~Shader();

public:
    GLuint GetProgram() const {
        return _Program;
    }

    GLvoid UseProgram() const {
        glUseProgram(_Program);
    }

    GLvoid SetUniform1i(const std::string& Name, GLboolean Value) const {
        glUniform1i(glGetUniformLocation(_Program, Name.c_str()), static_cast<GLint>(Value));
    }

    GLvoid SetUniform1i(const std::string& Name, GLint Value) const {
        glUniform1i(glGetUniformLocation(_Program, Name.c_str()), Value);
    }

    GLvoid SetUniform1f(const std::string& Name, GLfloat Value) const {
        glUniform1f(glGetUniformLocation(_Program, Name.c_str()), Value);
    }

    GLvoid SetUniform2f(const std::string& Name, GLfloat VectorX, GLfloat VectorY) const {
        glUniform2f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY);
    }

    GLvoid SetUniform3f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ) const {
        glUniform3f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY, VectorZ);
    }

    GLvoid SetUniform4f(const std::string& Name, GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ, GLfloat VectorW) const {
        glUniform4f(glGetUniformLocation(_Program, Name.c_str()), VectorX, VectorY, VectorZ, VectorW);
    }

    GLvoid SetUniform1fv(const std::string& Name, const std::vector<GLfloat>& Array) const {
        glUniform1fv(glGetUniformLocation(_Program, Name.c_str()), static_cast<GLsizei>(Array.size()), Array.data());
    }

    GLvoid SetUniform2fv(const std::string& Name, const glm::vec2& Vector) const {
        glUniform2fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
    }

    GLvoid SetUniform3fv(const std::string& Name, const glm::vec3& Vector) const {
        glUniform3fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
    }

    GLvoid SetUniform4fv(const std::string& Name, const glm::vec4& Vector) const {
        glUniform4fv(glGetUniformLocation(_Program, Name.c_str()), 1, glm::value_ptr(Vector));
    }

    GLvoid SetUniformMatrix2fv(const std::string& Name, const glm::mat2x2& Matrix) const {
        glUniformMatrix2fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
    }

    GLvoid SetUniformMatrix3fv(const std::string& Name, const glm::mat3x3& Matrix) const {
        glUniformMatrix3fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
    }

    GLvoid SetUniformMatrix4fv(const std::string& Name, const glm::mat4x4& Matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(_Program, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Matrix));
    }

private:
    std::string GetIncludeDirectory(const std::string& Filepath) const;
    std::string GetIncludeFilename(const std::string& Statement) const;
    Source LoadShaderSource(const std::string& Filepath);
    GLvoid InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const;
    GLuint CompileShader(const Source& ShaderSource, GLenum ShaderType) const;
    GLvoid LinkProgram(const std::vector<GLuint>& Shaders);

private:
    std::set<std::string> _IncludedFiles;
    std::vector<GLenum>   _ShaderTypes{ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER };
    GLuint                _Program{};
};

#endif // !ASSETLOADER_SHADER_H_

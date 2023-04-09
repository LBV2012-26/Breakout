#pragma warning(disable : 4715)

#include "Shader.h"

#include <cassert>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>

#include "GetAssetFilepath.h"

Shader::Shader(const std::vector<std::string>& SourceFiles, const std::vector<std::string>& Macros) {
    std::vector<Source> ShaderSources;
    for (const auto& kSourceFile : SourceFiles) {
        ShaderSources.emplace_back(LoadShaderSource(GetAssetFilepath(AssetType::kShader, kSourceFile)));
    }

    if (Macros.front() != "NULL") {
        for (GLuint i = 0; i != SourceFiles.size(); ++i) {
            InsertMacros(Macros, _ShaderTypes[i], ShaderSources[i]);
        }
    }

    std::vector<GLuint> Shaders;
    for (GLuint i = 0; i != ShaderSources.size(); ++i) {
        Shaders.emplace_back(CompileShader(ShaderSources[i], _ShaderTypes[i]));
    }

    LinkProgram(Shaders);

    for (GLuint Shader : Shaders) {
        glDetachShader(_Program, Shader);
        glDeleteShader(Shader);
    }
}

Shader::~Shader() {
    glDeleteProgram(_Program);
}

std::string Shader::GetIncludeDirectory(const std::string& Filepath) const {
    std::string::size_type LastSlashPosition = Filepath.find_last_of("/");
    if (LastSlashPosition == std::string::npos) {
        return "";
    } else {
        return Filepath.substr(0, LastSlashPosition + 1);
    }
}

std::string Shader::GetIncludeFilename(const std::string& Statement) const {
    std::string::size_type FirstQuotePosition = Statement.find_first_of("\"");
    std::string::size_type LastQuotePosition  = Statement.find_last_of("\"");

    return Statement.substr(FirstQuotePosition + 1, LastQuotePosition - FirstQuotePosition - 1);
}

Shader::Source Shader::LoadShaderSource(const std::string& Filepath) {
    std::ifstream SourceFile(Filepath);
    std::string   SourceCode;
    std::string   Statement;

    GLboolean bHasInclude = GL_FALSE;

    if (!SourceFile.is_open()) {
        std::cerr << std::format("Fatal error: Can not open shader source file: \"{}\": No such file or directory.", Filepath)
                  << std::endl;
        std::system("pause");
        std::exit(EXIT_FAILURE);
    }

    while (std::getline(SourceFile, Statement)) {
        if (Statement.find("#include") != std::string::npos) {
            bHasInclude = GL_TRUE;
            std::string IncludedFile = GetIncludeDirectory(Filepath) + GetIncludeFilename(Statement);
            if (_IncludedFiles.find(IncludedFile) == _IncludedFiles.end()) {
                _IncludedFiles.insert(IncludedFile);
                Statement = LoadShaderSource(IncludedFile).Data;
            } else {
                continue;
            }
        }

        SourceCode += Statement + '\n';
    }

    SourceFile.close();

    return { SourceCode, Filepath, bHasInclude, GL_FALSE };
}

GLvoid Shader::InsertMacros(const std::vector<std::string>& Macros, GLenum ShaderType, Source& ShaderSource) const {
    auto TypePrefix = [ShaderType]() -> std::string {
        switch (ShaderType) {
        case GL_VERTEX_SHADER:
            return "__VERT";
        case GL_FRAGMENT_SHADER:
            return "__FRAG";
        case GL_GEOMETRY_SHADER:
            return "__GEOM";
        default:
            assert(GL_FALSE);
        }
    }();

    GLuint                 InsertedMacroCount = 0;
    std::string::size_type InsertedCharLength = 0;

    for (const std::string& kMacro : Macros) {
        if (kMacro.find(TypePrefix) != std::string::npos) {
            ShaderSource.bHasMacros = GL_TRUE;
            ShaderSource.Data.insert(19ULL + InsertedCharLength, "#define " + kMacro.substr(7) + '\n');
            InsertedCharLength += kMacro.size() + 2;
            ++InsertedMacroCount;
        }
    }

    if (InsertedMacroCount) {
        ShaderSource.Data.insert(18ULL + InsertedCharLength, "\n");
    }
}

GLuint Shader::CompileShader(const Source& ShaderSource, GLenum ShaderType) const {
    GLuint Shader = glCreateShader(ShaderType);
    const GLchar* SourceCodePointer = ShaderSource.Data.c_str();

    glShaderSource(Shader, 1, &SourceCodePointer, nullptr);
    glCompileShader(Shader);

    GLint CompileStatus = 0;

    glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
    if (!CompileStatus) {
        GLint InfoLogLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::string InfoLog(InfoLogLength, '\0');
        glGetShaderInfoLog(Shader, InfoLogLength, nullptr, InfoLog.data());
        std::cerr << std::format("Shader \"{}\" compile failed:\n{}", ShaderSource.Filepath, InfoLog.data()) << std::endl;
        if (ShaderSource.bHasInclude || ShaderSource.bHasMacros) {
            std::ofstream ErrorShader(ShaderSource.Filepath + ".Error.glsl");
            ErrorShader << ShaderSource.Data << std::endl;
            ErrorShader.close();
        }

        std::system("pause");
        std::exit(EXIT_FAILURE);
    }

    return Shader;
}

GLvoid Shader::LinkProgram(const std::vector<GLuint>& Shaders) {
    _Program = glCreateProgram();

    for (GLuint Shader : Shaders) {
        glAttachShader(_Program, Shader);
    }

    glLinkProgram(_Program);

    GLint LinkStatus = 0;

    glGetProgramiv(_Program, GL_LINK_STATUS, &LinkStatus);
    if (!LinkStatus) {
        GLint InfoLogLength = 0;
        glGetProgramiv(_Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::string InfoLog(InfoLogLength, '\0');
        glGetProgramInfoLog(_Program, InfoLogLength, nullptr, InfoLog.data());
        std::cerr << std::format("Program link failed:\n{}", InfoLog.data()) << std::endl;

        std::system("pause");
        std::exit(EXIT_FAILURE);
    }
}

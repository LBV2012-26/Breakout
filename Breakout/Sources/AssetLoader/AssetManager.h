#pragma once
#ifndef ASSETLOADER_ASSETMANAGER_H_
#define ASSETLOADER_ASSETMANAGER_H_

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>

#include "Shader.h"
#include "Texture.h"

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager();

public:
    static GLvoid LoadShader(const std::string& Name,
                             const std::vector<std::string>& SourceFiles,
                             const std::vector<std::string>& Macros = { "NULL" }) {
        _kShaders[Name] = new Shader(SourceFiles, Macros);
    }

    static GLvoid LoadTexture(const std::string& Name, const std::string& ImageFilename) {
        _kTextures[Name] = new Texture2D(ImageFilename);
    }

    static const Shader* GetShader(const std::string& Name) {
        return _kShaders[Name];
    }

    static const Texture2D* GetTexture(const std::string& Name) {
        return _kTextures[Name];
    }

private:
    static std::map<std::string, const Shader*>    _kShaders;
    static std::map<std::string, const Texture2D*> _kTextures;
};

#endif // !ASSETLOADER_ASSETMANAGER_H_

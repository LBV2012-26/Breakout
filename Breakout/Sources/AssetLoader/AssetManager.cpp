#include "AssetManager.h"

std::map<std::string, const Shader*>    AssetManager::_kShaders;
std::map<std::string, const Texture2D*> AssetManager::_kTextures;

AssetManager::~AssetManager() {
    for (auto& Shader : _kShaders) {
        if (Shader.second) {
            delete Shader.second;
            Shader.second = nullptr;
        }
    }

    for (auto& Texture : _kTextures) {
        if (Texture.second) {
            delete Texture.second;
            Texture.second = nullptr;
        }
    }
}

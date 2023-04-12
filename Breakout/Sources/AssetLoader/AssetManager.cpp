#include "AssetManager.h"

std::map<std::string, std::shared_ptr<const Shader>>    AssetManager::_kShaders;
std::map<std::string, std::shared_ptr<const Texture2D>> AssetManager::_kTextures;

AssetManager::~AssetManager() {
    // Test smart pointer auto release.
    // --------------------------------
    // for (auto& Shader : _kShaders) {
    //     if (Shader.second) {
    //         delete Shader.second;
    //         Shader.second = nullptr;
    //     }
    // }

    // for (auto& Texture : _kTextures) {
    //     if (Texture.second) {
    //         delete Texture.second;
    //         Texture.second = nullptr;
    //     }
    // }
}

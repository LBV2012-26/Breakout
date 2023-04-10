#pragma warning(disable : 4715)

#include "GetAssetFilepath.h"
#include <cassert>
#include <glad/glad.h>

std::string GetAssetFilepath(AssetType Type, const std::string& Filename) {
    std::string RootFolderName = "Assets/";
#ifdef NDEBUG
    RootFolderName = std::string("../") + RootFolderName;
#endif // NDEBUG

    auto AssetFolderName = [Type]() -> std::string {
        switch (Type) {
        case AssetType::kFont:
            return "Fonts/";
        case AssetType::kLevel:
            return "Levels/";
        case AssetType::kShader:
            return "Shaders/";
        case AssetType::kSound:
            return "Music/";
        case AssetType::kTexture:
            return "Textures/";
        default:
            assert(GL_FALSE);
        }
    }();

    return RootFolderName + AssetFolderName + Filename;
}

#pragma once
#ifndef ASSETLOADER_GETASSETFILEPATH_H_
#define ASSETLOADER_GETASSETFILEPATH_H_

#include <string>

enum class AssetType {
    kFont, kLevel, kShader, kSound, kTexture
};

std::string GetAssetFilepath(AssetType Type, const std::string& Filename);

#endif // !ASSETLOADER_GETASSETFILEPATH_H_

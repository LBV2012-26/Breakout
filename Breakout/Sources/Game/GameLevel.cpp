#include "GameLevel.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "GLFW/glfw3.h"

#include "../AssetLoader/GetAssetFilepath.h"

GameLevel::GameLevel(const std::string& Filename, GLint LevelWidth, GLint LevelHeight) {
    _Bricks.clear();

    if (std::wcscmp(std::filesystem::path(Filename).extension().c_str(), L".level")) {
        return;
    }

    std::string   Filepath = GetAssetFilepath(AssetType::kLevel, Filename);
    std::ifstream LevelMap(Filepath);
    if (!LevelMap.is_open()) {
        std::cerr << std::format("Fatal error: can not open level map file: \"{}\": No such file or directory.", Filepath)
                  << std::endl;
        std::system("pause");
        std::exit(EXIT_FAILURE);
    }

    GLint       BrickCode = 0;
    GameLevel   Level;
    std::string Line;
    std::vector<std::vector<GLint>> BrickData;

    while (std::getline(LevelMap, Line)) {
        std::istringstream Stream(Line);
        std::vector<GLint> Row;
        while (Stream >> BrickCode) {
            Row.emplace_back(BrickCode);
        }

        BrickData.emplace_back(Row);
    }

    if (BrickData.size() > 0) {
        Init(BrickData, LevelWidth, LevelHeight);
    }
}

GameLevel::~GameLevel() {
    for (auto Brick : _Bricks) {
        if (Brick) {
            delete Brick;
            Brick = nullptr;
        }
    }
}

GLvoid GameLevel::Draw(const Sprite* Renderer) {
    for (const auto& kBrick : _Bricks) {
        if (!kBrick->GetDestructionState()) {
            kBrick->Draw(Renderer);
        }
    }
}

GLboolean GameLevel::IsDestroyed() {
    for (const auto& kBrick : _Bricks) {
        if (!kBrick->GetSolidState() && !kBrick->GetDestructionState()) {
            return GL_FALSE;
        }
    }

    return GL_TRUE;
}

GLvoid GameLevel::Init(const std::vector<std::vector<GLint>>& BrickData, GLint LevelWidth, GLint LevelHeight) {
    GLint   Width      = static_cast<GLint>(BrickData.front().size());
    GLint   Height     = static_cast<GLint>(BrickData.size());
    GLfloat UnitWidth  = LevelWidth  / static_cast<GLfloat>(Width);
    GLfloat UnitHeight = LevelHeight / static_cast<GLfloat>(Height);

    for (GLint y = 0; y != Height; ++y) {
        for (GLint x = 0; x != Width; ++x) {
            if (BrickData[y][x] == 1) {
                glm::vec2 Position(UnitWidth * x, UnitHeight * y);
                glm::vec2 Size(UnitWidth, UnitHeight);
                GameObject* Brick = new GameObject(Position, Size, AssetManager::GetTexture("Solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                Brick->SetSolidState(GL_TRUE);
                _Bricks.emplace_back(Brick);
            } else if (BrickData[y][x] > 1) {
                auto Color = [&BrickData, x, y]() -> glm::vec3 {
                    if (BrickData[y][x] == 2)
                        return { 0.2f, 0.6f, 1.0f };
                    else if (BrickData[y][x] == 3)
                        return { 0.0f, 0.7f, 0.0f };
                    else if (BrickData[y][x] == 4)
                        return { 0.8f, 0.8f, 0.4f };
                    else if (BrickData[y][x] == 5)
                        return { 1.0f, 0.5f, 0.0f };
                    else
                        return { 0, 0, 0 };
                }();

                glm::vec2 Position(UnitWidth * x, UnitHeight * y);
                glm::vec2 Size(UnitWidth, UnitHeight);
                _Bricks.emplace_back(new GameObject(Position, Size, AssetManager::GetTexture("Block"), Color));
            }
        }
    }
}

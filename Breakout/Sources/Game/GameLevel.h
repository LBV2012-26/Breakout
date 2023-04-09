#pragma once
#ifndef GAME_GAMELEVEL_H_
#define GAME_GAMELEVEL_H_

#include <vector>
#include <glad/glad.h>

#include "GameObject/GameObject.h"
#include "Renderer/Sprite.h"
#include "../AssetLoader/AssetManager.h"

class GameLevel {
private:
    using BrickLayout = std::vector<std::vector<GLint>>;

public:
    GameLevel() = default;
    GameLevel(const std::string& Filename, GLint LevelWidth, GLint LevelHeight);
    ~GameLevel();

    GLvoid    Draw(const Sprite* Renderer);
    GLboolean IsDestroyed();

public:
    std::vector<GameObject*>& GetBricks() {
        return _Bricks;
    }

private:
    GLvoid Init(const std::vector<std::vector<GLint>>& BrickData, GLint LevelWidth, GLint LevelHeight);

private:
    std::vector<GameObject*> _Bricks;
};

#endif // !GAME_GAMELEVEL_H_

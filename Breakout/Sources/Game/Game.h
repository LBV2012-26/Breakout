#pragma once
#ifndef GAME_GAME_H_
#define GAME_GAME_H_

#include <array>
#include <tuple>
#include <vector>

#include <glad/glad.h>
#include <irrKlang/irrKlang.h>

#include "GameObject/BallObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/PowerUp.hpp"
#include "Renderer/ParticleGenerator.h"
#include "Renderer/PostProcessor.h"
#include "Renderer/Sprite.h"
#include "Renderer/TextRenderer.h"
#include "GameLevel.h"

class Game {
private:
    enum class GameState {
        kActivated, kMenu, kWon
    };

    enum class Direction {
        kUp, kRight, kDown, kLeft
    };

    using Collision = std::tuple<GLboolean, Direction, glm::vec2>;

public:
    Game() = default;
    Game(GLint WindowWidth, GLint WindowHeight);
    ~Game();

    GLvoid ProcessInput(GLfloat DeltaTime);
    GLvoid Update(GLfloat DeltaTime);
    GLvoid Render();

public:
    GLvoid SetKey(GLint Index, GLboolean bValue) {
        _Keys[Index] = bValue;
    }

    GLvoid SetProcessedKey(GLint Index, GLboolean bValue) {
        _ProcessedKeys[Index] = bValue;
    }

    GLvoid ResetPlayer() {
        _Paddle->SetPosition(glm::vec2(_WindowWidth / 2.0f - _PaddleSize.x / 2.0f, _WindowHeight - _PaddleSize.y));
        _Ball->Reset(_Paddle->GetPosition() + glm::vec2(_PaddleSize.x / 2.0f - _BallRadius, -_BallRadius * 2.0f), _BallVelocity);
    }

private:
    Collision CheckCollision(const BallObject* Ball, const GameObject* Brick);
    GLboolean CheckCollision(const PowerUp* PowerUp, const GameObject* Paddle);
    Direction VectorDirection(const glm::vec2& Target);
    GLvoid    Collide();
    GLboolean ShouldSpawnPowerUp(GLint Chance);
    GLvoid    SpawnPowerUps(const GameObject* Brick);
    GLvoid    ActivatePowerUp(const PowerUp* CollidedPowerUp);
    GLboolean IsOtherPowerUpActivated(PowerUpType Type);
    GLvoid    UpdatePowerUps(GLfloat DeltaTime);
    GLvoid    ResetLevel();

private:
    std::array<GLboolean, 1024> _Keys;
    std::array<GLboolean, 1024> _ProcessedKeys;
    GLfloat                     _WindowWidth;
    GLfloat                     _WindowHeight;
    GameState                   _State;
    GLint                       _Lives;
    GLint                       _LevelIndex;
    std::vector<GameLevel*>     _Levels;
    std::vector<PowerUp*>       _PowerUps;
    GLfloat                     _ShakeTime;
    glm::vec2                   _PaddleVelocity;
    glm::vec2                   _PaddleSize;
    glm::vec2                   _BallVelocity;
    GLfloat                     _BallRadius;
    GLint                       _StickyChance;

    AssetManager*               _Assets;
    Sprite*                     _Renderer;
    GameObject*                 _Paddle;
    BallObject*                 _Ball;
    ParticleGenerator*          _Particle;
    PostProcessor*              _PostEffect;
    TextRenderer*               _Text;

    irrklang::ISoundEngine*     _SoundEngine;
};

#endif // !GAME_GAME_H_

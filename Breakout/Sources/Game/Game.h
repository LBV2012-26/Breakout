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
#include "Renderer/SpriteRenderer.h"
#include "Renderer/TextRenderer.h"
#include "GameLevel.h"

constexpr glm::vec2 kPaddleVelocity = glm::vec2(500.0f);
constexpr glm::vec2 kPaddleSize = glm::vec2(150.0f, 30.0f);
constexpr glm::vec2 kBallVelocity = glm::vec2(150.0f, -525.0f);
constexpr GLfloat   kBallRadius = 25.0f;

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
    GLvoid    ResetPlayer();
    GLvoid    ClearAllPositivePowerUps();

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
    GLint                       _StickyChance;

    AssetManager*               _Assets;
    SpriteRenderer*             _Sprite;
    GameObject*                 _Paddle;
    BallObject*                 _Ball;
    ParticleGenerator*          _Particle;
    PostProcessor*              _PostEffect;
    TextRenderer*               _Text;

    irrklang::ISoundEngine*     _SoundEngine;
};

#endif // !GAME_GAME_H_

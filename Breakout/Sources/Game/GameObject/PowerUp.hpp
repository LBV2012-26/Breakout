#pragma once
#ifndef GAME_GAMEOBJECT_POWERUP_HPP_
#define GAME_GAMEOBJECT_POWERUP_HPP_

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"

constexpr glm::vec2 _kPowerUpSize = glm::vec2(90.0f, 30.0f);
constexpr glm::vec2 _kVelocity    = glm::vec2(0.0f, 225.0f);

enum class PowerUpType {
    kChaos, kConfuse, kPadSizeIncrease, kPassThrough, kSpeed, kSticky
};

class PowerUp : public GameObject {
public:
    PowerUp() = default;
    PowerUp(const glm::vec2& Position, const Texture2D* SpriteTex, const glm::vec3& Color, PowerUpType Type, GLfloat Duration) :
        GameObject(Position, _kPowerUpSize, SpriteTex, Color, _kVelocity), _Type(Type), _Duration(Duration), _bIsActivated(GL_FALSE) {}

public:
    PowerUpType GetPowerUpType() const {
        return _Type;
    }

    GLboolean GetActiveState() const {
        return _bIsActivated;
    }

    GLfloat GetDuration() const {
        return _Duration;
    }

    GLvoid SetActiveState(GLboolean bValue) {
        _bIsActivated = bValue;
    }

    GLvoid SetDuration(GLfloat Value) {
        _Duration = Value;
    }

private:
    PowerUpType _Type;
    GLfloat     _Duration;
    GLboolean   _bIsActivated;
};

#endif // !GAME_GAMEOBJECT_POWERUP_HPP_

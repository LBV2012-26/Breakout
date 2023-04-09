#pragma once
#ifndef GAME_GAMEOBJECT_BALLOBJECT_H_
#define GAME_GAMEOBJECT_BALLOBJECT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "../../AssetLoader/Texture.h"

class BallObject : public GameObject {
public:
    BallObject();
    BallObject(const glm::vec2& Position, GLfloat Radius, const glm::vec2& Velocity, const Texture2D* SpriteTex);
    ~BallObject();

    glm::vec2 Move(GLfloat DeltaTime, GLint WindowWidth);
    GLvoid    Reset(const glm::vec2& Position, const glm::vec2& Velocity);

public:
    GLfloat GetRadius() const {
        return _Radius;
    }

    GLboolean GetPassThroughState() const {
        return _PassThrough;
    }

    GLboolean GetStickyState() const {
        return _Sticky;
    }

    GLboolean GetStuckState() const {
        return _Stuck;
    }

    GLvoid SetRadius(GLfloat Value) {
        _Radius = Value;
    }

    GLvoid SetPassThroughState(GLboolean bValue) {
        _PassThrough = bValue;
    }

    GLvoid SetStickyState(GLboolean bValue) {
        _Sticky = bValue;
    }

    GLvoid SetStuckState(GLboolean bValue) {
        _Stuck = bValue;
    }

private:
    GLfloat   _Radius;
    GLboolean _PassThrough;
    GLboolean _Sticky;
    GLboolean _Stuck;
};

#endif // !GAME_GAMEOBJECT_BALLOBJECT_H_

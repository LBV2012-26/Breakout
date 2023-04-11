#include "BallObject.h"
#include "../../AssetLoader/AssetManager.h"

BallObject::BallObject() : GameObject(), _Radius(12.5f), _bStuck(GL_TRUE) {}
BallObject::BallObject(const glm::vec2& Position, GLfloat Radius, const glm::vec2& Velocity, const Texture2D* SpriteTex) :
    GameObject(Position, glm::vec2(Radius * 2.0f), SpriteTex, glm::vec3(1.0f), Velocity), _Radius(Radius), _bStuck(GL_TRUE), _bSticky(GL_FALSE) {}

BallObject::~BallObject() {
    // Don't delete pointer there
    // The memory will be released when AssetManager execute destruct function.
    // -----------------------------------------------------------------------
    // if (_SpriteTex) {
    //     delete _SpriteTex;
    //     _SpriteTex = nullptr;
    // }
}

glm::vec2 BallObject::Move(GLfloat DeltaTime, GLint WindowWidth) {
    if (!_bStuck) {
        _Position += _Velocity * DeltaTime;

        if (_Position.x <= 0.0f) {
            _Velocity.x = -_Velocity.x;
            _Position.x = 0.0f;
        } else if (_Position.x + _Size.x >= WindowWidth) {
            _Velocity.x = -_Velocity.x;
            _Position.x = WindowWidth - _Size.x;
        }

        if (_Position.y <= 0.0f) {
            _Velocity.y = -_Velocity.y;
            _Position.y = 0.0f;
        }
    }

    return _Position;
}

GLvoid BallObject::Reset(const glm::vec2& Position, const glm::vec2& Velocity) {
    _Position = Position;
    _Velocity = Velocity;
    _bStuck   = GL_TRUE;
}

#pragma once
#ifndef GAME_GAMEOBJECT_GAMEOBJECT_H_
#define GAME_GAMEOBJECT_GAMEOBJECT_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Renderer/Sprite.h"
#include "../../AssetLoader/Texture.h"

class GameObject {
public:
    GameObject();
    GameObject(const glm::vec2& Position,
               const glm::vec2& Size,
               const Texture2D* SpriteTex,
               const glm::vec3& Color    = glm::vec3(1.0f),
               const glm::vec2& Velocity = glm::vec2(0.0f));
    ~GameObject();

    virtual GLvoid Draw(const Sprite* Renderer) const;

public:
    const glm::vec2& GetPosition() const {
        return _Position;
    }

    const glm::vec2& GetSize() const {
        return _Size;
    }

    const glm::vec3& GetColor() const {
        return _Color;
    }

    const glm::vec2& GetVelocity() const {
        return _Velocity;
    }

    GLfloat GetRotation() const {
        return _Rotation;
    }

    GLboolean GetSolidState() const {
        return _bIsSolid;
    }

    GLboolean GetDestructionState() const {
        return _bHasBeenDestroyed;
    }

    GLvoid SetPosition(const glm::vec2& Value) {
        _Position = Value;
    }

    GLvoid SetPositionX(GLfloat Value) {
        _Position.x = Value;
    }

    GLvoid SetPositionY(GLfloat Value) {
        _Position.y = Value;
    }

    GLvoid SetSize(const glm::vec2& Value) {
        _Size = Value;
    }

    GLvoid SetSizeX(GLfloat Value) {
        _Size.x = Value;
    }

    GLvoid SetSizeY(GLfloat Value) {
        _Size.y = Value;
    }

    GLvoid SetColor(const glm::vec3& Value) {
        _Color = Value;
    }

    GLvoid SetVelocity(const glm::vec2& Value) {
        _Velocity = Value;
    }

    GLvoid SetVelocityX(GLfloat Value) {
        _Velocity.x = Value;
    }

    GLvoid SetVelocityY(GLfloat Value) {
        _Velocity.y = Value;
    }

    GLvoid SetRotation(GLfloat Value) {
        _Rotation = Value;
    }

    GLvoid SetSolidState(GLboolean bValue) {
        _bIsSolid = bValue;
    }

    GLvoid SetDestructionState(GLboolean bValue) {
        _bHasBeenDestroyed = bValue;
    }

protected:
    glm::vec2  _Position;
    glm::vec2  _Size;
    glm::vec3  _Color;
    glm::vec2  _Velocity;

    GLfloat    _Rotation;
    GLboolean  _bIsSolid;
    GLboolean  _bHasBeenDestroyed;

    const Texture2D* _SpriteTex;
};

#endif // !GAME_GAMEOBJECT_GAMEOBJECT_H_

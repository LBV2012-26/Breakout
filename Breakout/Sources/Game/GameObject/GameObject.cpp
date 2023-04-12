#include "GameObject.h"
#include "../../AssetLoader/AssetManager.h"

GameObject::GameObject() :
    _Position(0.0f), _Size(1.0f),         _Color(1.0f),                 _Velocity(0.0f),
    _Rotation(0.0f), _bIsSolid(GL_FALSE), _bHasBeenDestroyed(GL_FALSE), _SpriteTex(nullptr) {}

GameObject::GameObject(const glm::vec2& Position, const glm::vec2& Size, const Texture2D* SpriteTex,
                       const glm::vec3& Color,    const glm::vec2& Velocity) :
    _Position(Position), _Size(Size),         _Color(Color),                _Velocity(Velocity),
    _Rotation(0.0f),     _bIsSolid(GL_FALSE), _bHasBeenDestroyed(GL_FALSE), _SpriteTex(SpriteTex) {}

GameObject::~GameObject() {
    // The memory will release after AssetManager execute destruction function.
    // ------------------------------------------------------------------------
    // if (_SpriteTex) {
    //     delete _SpriteTex;
    //     _SpriteTex = nullptr;
    // }
}

GLvoid GameObject::Draw(const SpriteRenderer* Sprite) const {
    Sprite->Draw(_SpriteTex, _Position, _Size, _Rotation, _Color);
}

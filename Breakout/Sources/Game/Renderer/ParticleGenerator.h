#pragma once
#ifndef GAME_RENDERER_PARTICLEGENERATOR_H_
#define GAME_RENDERER_PARTICLEGENERATOR_H_

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../AssetLoader/Shader.h"
#include "../../AssetLoader/Texture.h"
#include "../GameObject/GameObject.h"

class ParticleGenerator {
private:
    struct Particle {
        Particle() : Position(0.0f), Velocity(0.0f), Bright(1.0f), Life(0.0f) {}

        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 Bright;
        GLfloat   Life;
    };

public:
    ParticleGenerator() = default;
    ParticleGenerator(const Shader* ParticleShader, const Texture2D* ParticleTex, GLint Amount);
    ~ParticleGenerator();

    GLvoid Update(GLfloat DeltaTime, const GameObject* Object, GLint NewParticles, const glm::vec2& Offset = glm::vec2(0.0f));
    GLvoid Draw();

    GLint  FirstUnusedParticle();
    GLvoid RespawnParticle(const GameObject* Object, const glm::vec2& Offset, Particle& UnusedParticle);

public:
    GLvoid SetBurnState(GLboolean bValue) {
        _bBurned = bValue;
    }

private:
    std::vector<Particle> _Particles;
    GLint                 _Amount;
    GLint                 _LastUsedParticle;
    GLuint                _VertexArray;
    GLboolean             _bBurned;
    const Shader*         _ParticleShader;
    const Texture2D*      _ParticleTex;
};

#endif // !GAME_RENDERER_PARTICLEGENERATOR_H_

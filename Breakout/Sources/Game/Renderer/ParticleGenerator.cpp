#include "ParticleGenerator.h"
#include <random>

#include "../../AssetLoader/AssetManager.h"
#include "../../Constants.h"

ParticleGenerator::ParticleGenerator(const Shader* ParticleShader, const Texture2D* ParticleTex, GLint Amount) :
    _ParticleShader(ParticleShader), _ParticleTex(ParticleTex), _Amount(Amount), _LastUsedParticle(0) {
#include "Vertices.inc"

    GLuint VertexBuffer = 0;

    glCreateVertexArrays(1, &_VertexArray);
    glCreateBuffers(1, &VertexBuffer);

    glNamedBufferData(VertexBuffer, Vertices.size() * sizeof(GLfloat), Vertices.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(_VertexArray, 0, VertexBuffer, 0, 4 * sizeof(GLfloat));

    glEnableVertexArrayAttrib(_VertexArray, 0);
    glVertexArrayAttribFormat(_VertexArray, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VertexArray, 0, 0);

    for (GLint i = 0; i != Amount; ++i) {
        _Particles.emplace_back(Particle());
    }

    glDeleteBuffers(1, &VertexBuffer);
}

ParticleGenerator::~ParticleGenerator() {
    glDeleteVertexArrays(1, &_VertexArray);

    // Don't delete pointer there
    // The memory will be released when AssetManager execute destruct function.
    // -----------------------------------------------------------------------
    // if (_ParticleShader) {
    //     delete _ParticleShader;
    //     _ParticleShader = nullptr;
    // }

    // if (_ParticleTex) {
    //     delete _ParticleTex;
    //     _ParticleTex = nullptr;
    // }
}

GLvoid ParticleGenerator::Update(GLfloat DeltaTime, const GameObject* Object, GLint NewParticles, const glm::vec2& Offset) {
    for (GLint i = 0; i != NewParticles; ++i) {
        GLint LastUsedParticle = FirstUnusedParticle();
        RespawnParticle(Object, Offset, _Particles[LastUsedParticle]);
    }

    for (auto& Particle : _Particles) {
        Particle.Life -= DeltaTime;
        if (Particle.Life > 0.0f) {
            Particle.Position -= Particle.Velocity * DeltaTime;
            Particle.Bright.a -= DeltaTime * 2.0f;
        }
    }
}

GLvoid ParticleGenerator::Draw() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    _ParticleShader->UseProgram();
    for (const auto& kParticle : _Particles) {
        if (kParticle.Life > 0.0f) {
            _ParticleShader->SetUniform1i("bBurned", _bBurned);
            _ParticleShader->SetUniform2fv("Offset", kParticle.Position);
            _ParticleShader->SetUniform4fv("ParticleBright", kParticle.Bright);
            _ParticleTex->BindTextureUnit(_ParticleShader, "ParticleTex", kParticleTexUnit);
            glBindVertexArray(_VertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLint ParticleGenerator::FirstUnusedParticle() {
    for (GLint i = _LastUsedParticle; i < _Amount; ++i) {
        if (_Particles[i].Life <= 0.0f) {
            _LastUsedParticle = i;
            return i;
        }
    }

    for (GLint i = 0; i != _LastUsedParticle; ++i) {
        if (_Particles[i].Life <= 0.0f) {
            _LastUsedParticle = i;
            return i;
        }
    }

    _LastUsedParticle = 0;
    return 0;
}

GLvoid ParticleGenerator::RespawnParticle(const GameObject* Object, const glm::vec2& Offset, Particle& UnusedParticle) {
    std::mt19937 RandomEngine;
    RandomEngine.seed(std::random_device{}());
    std::uniform_real_distribution<GLfloat> RandomGenerator(0.0f, 100.0f);
    GLfloat Random      = (RandomGenerator(RandomEngine) - 50) / 10.0f;
    GLfloat RandomColor =  RandomGenerator(RandomEngine) / 100.0f + 0.5f;

    UnusedParticle.Position = Object->GetPosition() + Random + Offset;
    UnusedParticle.Bright   = glm::vec4(glm::vec3(RandomColor), 1.0f);
    UnusedParticle.Life     = 1.0f;
    UnusedParticle.Velocity = Object->GetVelocity() * 0.1f;
}

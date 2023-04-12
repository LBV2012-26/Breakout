#include "Game.h"

#include <filesystem>
#include <limits>
#include <random>
#include <string>

#include <GLFW/glfw3.h>

#include "Renderer/SpriteRenderer.h"
#include "../AssetLoader/AssetManager.h"
#include "../AssetLoader/GetAssetFilepath.h"
#include "../Constants.h"

Game::Game(GLint WindowWidth, GLint WindowHeight) :
    _Keys(), _State(GameState::kMenu), _Lives(3), _ShakeTime(0.0f), _LevelIndex(0), _StickyChance(0), _PosPowerUpChance(50), _NegPowerUpChance(25),
    _WindowWidth(static_cast<GLfloat>(WindowWidth)), _WindowHeight(static_cast<GLfloat>(WindowHeight)),
    _Assets(new AssetManager), _Sprite(nullptr), _Paddle(nullptr), _Ball(nullptr), _PostEffect(nullptr), _SoundEngine(nullptr) {
    _Assets->LoadShader("Sprite",     { "Vertex.glsl",             "Sprite.glsl" });
    _Assets->LoadShader("Particle",   { "Vertex.glsl",             "Particle.glsl" }, { "__VERT_PARTICLE" });
    _Assets->LoadShader("Text",       { "Vertex.glsl",             "Text.glsl" },     { "__VERT_TEXT" });
    _Assets->LoadShader("PoseEffect", { "ScreenBufferVertex.glsl", "PostProcessor.glsl" });

    glm::mat4x4 Projection = glm::ortho(0.0f, _WindowWidth, _WindowHeight, 0.0f, -1.0f, 0.0f);
    _Assets->GetShader("Sprite")->UseProgram();
    _Assets->GetShader("Sprite")->SetUniformMatrix4fv("Projection", Projection);
    _Assets->GetShader("Particle")->UseProgram();
    _Assets->GetShader("Particle")->SetUniformMatrix4fv("Projection", Projection);

    _Assets->LoadTexture("Background",      "Background.jpg");
    _Assets->LoadTexture("Face",            "AwesomeFace.png");
    _Assets->LoadTexture("Block",           "Block.png");
    _Assets->LoadTexture("Solid",           "Solid.png");
    _Assets->LoadTexture("Paddle",          "Paddle.png");
    _Assets->LoadTexture("Particle",        "Particle.png");
    _Assets->LoadTexture("Chaos",           "PowerUps/Chaos.png");
    _Assets->LoadTexture("Confuse",         "PowerUps/Confuse.png");
    _Assets->LoadTexture("PadSizeIncrease", "PowerUps/PadSizeIncrease.png");
    _Assets->LoadTexture("PassThrough",     "PowerUps/PassThrough.png");
    _Assets->LoadTexture("Speed",           "PowerUps/Speed.png");
    _Assets->LoadTexture("Sticky",          "PowerUps/Sticky.png");

    _Assets->GetTexture("Face")->BindTextureUnit(_Assets->GetShader("Sprite"), "SpriteTex", kSpriteTexUnit);
    _Assets->GetTexture("Particle")->BindTextureUnit(_Assets->GetShader("Particle"), "ParticleTex", kParticleTexUnit);

    _Sprite     = new SpriteRenderer(_Assets->GetShader("Sprite"));
    _Particle   = new ParticleGenerator(_Assets->GetShader("Particle"), _Assets->GetTexture("Particle"), 500);
    _PostEffect = new PostProcessor(_Assets->GetShader("PoseEffect"), static_cast<GLsizei>(_WindowWidth), static_cast<GLsizei>(_WindowHeight), kMultiSamples);
    _Text       = new TextRenderer(_Assets->GetShader("Text"), "Monaco.ttf", static_cast<GLsizei>(_WindowWidth), static_cast<GLsizei>(_WindowHeight), 24);


    std::string LevelAssetDirectory = GetAssetFilepath(AssetType::kLevel, "");
    for (const auto& kEntry : std::filesystem::directory_iterator(LevelAssetDirectory)) {
        GameLevel* Level = new GameLevel(kEntry.path().filename().string(), static_cast<GLint>(_WindowWidth), static_cast<GLint>(_WindowHeight / 2.0f));
        _Levels.emplace_back(Level);
    }

    _LevelIndex = 0;

    glm::vec2 PaddlePos(_WindowWidth / 2.0f - kPaddleSize.x / 2.0f, _WindowHeight - kPaddleSize.y);
    _Paddle = new GameObject(PaddlePos, kPaddleSize, _Assets->GetTexture("Paddle"), glm::vec3(1.0f), kPaddleVelocity);

    glm::vec2 BallPos = PaddlePos + glm::vec2(_Paddle->GetSize().x / 2.0f - kBallRadius, -kBallRadius * 2.0f);
    _Ball = new BallObject(BallPos, kBallRadius, kBallVelocity, _Assets->GetTexture("Face"));

    _SoundEngine = irrklang::createIrrKlangDevice();
    _SoundEngine->play2D(GetAssetFilepath(AssetType::kSound, "Breakout.mp3").c_str(), GL_TRUE);
}

Game::~Game() {
    for (auto Level : _Levels) {
        if (Level) {
            delete Level;
            Level = nullptr;
        }
    }

    for (auto PowerUp : _PowerUps) {
        if (PowerUp) {
            delete PowerUp;
            PowerUp = nullptr;
        }
    }

    if (_Sprite) {
        delete _Sprite;
        _Sprite = nullptr;
    }

    if (_Paddle) {
        delete _Paddle;
        _Paddle = nullptr;
    }

    if (_Ball) {
        delete _Ball;
        _Ball = nullptr;
    }

    if (_Particle) {
        delete _Particle;
        _Particle = nullptr;
    }

    if (_PostEffect) {
        delete _PostEffect;
        _PostEffect = nullptr;
    }

    if (_SoundEngine) {
        _SoundEngine->drop();
    }

    if (_Assets) {
        delete _Assets;
        _Assets = nullptr;
    }
}

GLvoid Game::ProcessInput(GLfloat DeltaTime) {
    if (_State == GameState::kActivated) {
        GLfloat Velocity = _Paddle->GetVelocity().x * DeltaTime;
        if (_Keys[GLFW_KEY_A] || _Keys[GLFW_KEY_LEFT]) {
            if (_Paddle->GetPosition().x >= 0.0f) {
                _Paddle->SetPositionX(_Paddle->GetPosition().x - Velocity);
                if (_Ball->GetStuckState()) {
                    _Ball->SetPositionX(_Ball->GetPosition().x - Velocity);
                }
            }
        }

        if (_Keys[GLFW_KEY_D] || _Keys[GLFW_KEY_RIGHT]) {
            if (_Paddle->GetPosition().x <= _WindowWidth - _Paddle->GetSize().x) {
                _Paddle->SetPositionX(_Paddle->GetPosition().x + Velocity);
                if (_Ball->GetStuckState()) {
                    _Ball->SetPositionX(_Ball->GetPosition().x + Velocity);
                }
            }
        }

        if (_Keys[GLFW_KEY_SPACE] && !_ProcessedKeys[GLFW_KEY_SPACE]) {
            _Ball->SetStuckState(GL_FALSE);
            _ProcessedKeys[GLFW_KEY_SPACE] = GL_TRUE;
            if (_StickyChance > 0) {
                --_StickyChance;
            }
        }
    }

    if (_State == GameState::kMenu) {
        if (_Keys[GLFW_KEY_W] && !_ProcessedKeys[GLFW_KEY_W]) {
            _LevelIndex = (_LevelIndex + 1) % (static_cast<GLint>(_Levels.size()) - 1);
            _ProcessedKeys[GLFW_KEY_W] = GL_TRUE;
        }

        if (_Keys[GLFW_KEY_S] && !_ProcessedKeys[GLFW_KEY_S]) {
            if (_LevelIndex > 0) {
                --_LevelIndex;
            } else {
                _LevelIndex = static_cast<GLint>(_Levels.size()) - 2;
            }

            _ProcessedKeys[GLFW_KEY_S] = GL_TRUE;
        }

        if (_Keys[GLFW_KEY_ENTER] && !_ProcessedKeys[GLFW_KEY_ENTER]) {
            _State = GameState::kActivated;
            _ProcessedKeys[GLFW_KEY_ENTER] = GL_TRUE;
        }
    }

    if (_State == GameState::kWon) {
        if (_Keys[GLFW_KEY_ENTER] && !_ProcessedKeys[GLFW_KEY_ENTER]) {
            _State = GameState::kMenu;
            _ProcessedKeys[GLFW_KEY_ENTER] = GL_TRUE;
            _PostEffect->SetEffectState(Effects::kChaos, GL_FALSE);
        }
    }
}

GLvoid Game::Update(GLfloat DeltaTime) {
    _Ball->Move(DeltaTime, static_cast<GLint>(_WindowWidth));
    Collide();
    _Particle->Update(DeltaTime, _Ball, 2, glm::vec2(_Ball->GetRadius() / 2.0f));
    UpdatePowerUps(DeltaTime);

    if (_ShakeTime > 0.0f) {
        _ShakeTime -= DeltaTime;
        if (_ShakeTime <= 0.0f) {
            _PostEffect->SetEffectState(Effects::kShake, GL_FALSE);
        }
    }

    if (_Ball->GetPosition().y >= _WindowHeight) {
        --_Lives;
        ResetPlayer();
        if (_Lives == 0) {
            ResetLevel();
        }
    }

    if (_Levels[_LevelIndex]->IsCompleted() && _State == GameState::kActivated) {
        ResetLevel();
        ResetPlayer();
        _PostEffect->SetEffectState(Effects::kChaos, GL_TRUE);
        _State = GameState::kWon;
    }
}

GLvoid Game::Render() {
    _PostEffect->BeginRender();

    _Sprite->Draw(AssetManager::GetTexture("Background"), glm::vec2(0.0f), glm::vec2(_WindowWidth, _WindowHeight));
    _Levels[_LevelIndex]->Draw(_Sprite);
    _Paddle->Draw(_Sprite);

    for (const auto PowerUp : _PowerUps) {
        if (!PowerUp->GetDestructionState()) {
            PowerUp->Draw(_Sprite);
        }
    }

    _Particle->Draw();
    _Ball->Draw(_Sprite);

    _PostEffect->EndRender();
    _PostEffect->Render(static_cast<GLfloat>(glfwGetTime()));

    _Text->Draw("Lives: " + std::to_string(_Lives), 5.0f, 5.0f, 1.0f);

    if (_State == GameState::kMenu) {
        _Text->Draw("Press [Enter] to start.",                    400.0f, _WindowHeight / 2.0f, 1.5f);
        _Text->Draw("Press [W] or [S] to select level.",          410.0f, _WindowHeight / 2.0f + 50.0f, 1.0f);
        _Text->Draw("When game started press [Space] to launch.", 350.0f, _WindowHeight / 2.0f + 90.0f, 1.0f);
    }

    if (_State == GameState::kWon) {
        _Text->Draw("You Won! XD",                              480.0f, _WindowHeight / 2.0f - 50.0f, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        _Text->Draw("Press [Enter] to retry or [Esc] to quit.", 360.0f, _WindowHeight / 2.0f,         1.0,  glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

Game::Collision Game::CheckCollision(const BallObject* Ball, const GameObject* Brick) {
    glm::vec2 BallCenter(Ball->GetPosition() + Ball->GetRadius());
    glm::vec2 BrickHalfExtents(Brick->GetSize().x / 2.0f, Brick->GetSize().y / 2.0f);
    glm::vec2 BrickCenter(Brick->GetPosition().x + BrickHalfExtents.x,
                          Brick->GetPosition().y + BrickHalfExtents.y);
    glm::vec2 Difference = BallCenter - BrickCenter;
    glm::vec2 Clamped    = glm::clamp(Difference, -BrickHalfExtents, BrickHalfExtents);
    glm::vec2 Closest    = BrickCenter + Clamped;
    Difference = Closest - BallCenter;

    if (glm::length(Difference) <= Ball->GetRadius()) {
        return Collision(GL_TRUE, VectorDirection(Difference), Difference);
    } else {
        return Collision(GL_FALSE, Direction::kUp, glm::vec2(0.0f));
    }
}

GLboolean Game::CheckCollision(const PowerUp* PowerUp, const GameObject* Paddle) {
    GLboolean CollisionX = PowerUp->GetPosition().x + PowerUp->GetSize().x >= Paddle->GetPosition().x &&
        Paddle->GetPosition().x + Paddle->GetSize().x >= PowerUp->GetPosition().x;
    GLboolean CollisionY = PowerUp->GetPosition().y + PowerUp->GetSize().y >= Paddle->GetPosition().y &&
        Paddle->GetPosition().y + Paddle->GetSize().y >= PowerUp->GetPosition().y;
    return CollisionX & CollisionY;
}

Game::Direction Game::VectorDirection(const glm::vec2& Target) {
    std::vector<glm::vec2> Compass{
        glm::vec2( 0.0f,  1.0f),
        glm::vec2( 1.0f,  0.0f),
        glm::vec2( 0.0f, -1.0f),
        glm::vec2(-1.0f,  0.0f)
    };

    GLfloat Maximum = 0.0f;
    GLint   BestMatch = -1;
    for (GLint i = 0; i != 4; ++i) {
        GLfloat DotProduct = glm::dot(glm::normalize(Target), Compass[i]);
        if (DotProduct > Maximum) {
            Maximum = DotProduct;
            BestMatch = i;
        }
    }

    return static_cast<Direction>(BestMatch);
}

GLvoid Game::Collide() {
    Collision BallBrick;
    Direction CollisionDir;
    for (auto& Brick : _Levels[_LevelIndex]->GetBricks()) {
        BallBrick = CheckCollision(_Ball, Brick);
        if (!Brick->GetDestructionState()) {
            if (std::get<0>(BallBrick)) {
                if (!Brick->GetSolidState()) {
                    Brick->SetDestructionState(GL_TRUE);
                    SpawnPowerUps(Brick);
                    _SoundEngine->play2D(GetAssetFilepath(AssetType::kSound, "Block.mp3").c_str());
                } else {
                    _ShakeTime = 0.05f;
                    _PostEffect->SetEffectState(Effects::kShake, GL_TRUE);
                    _SoundEngine->play2D(GetAssetFilepath(AssetType::kSound, "Solid.wav").c_str());
                }

                CollisionDir = std::get<1>(BallBrick);
                glm::vec2 DiffVector = std::get<2>(BallBrick);
                if (!(_Ball->GetPassThroughState() && !Brick->GetSolidState())) {
                    if (CollisionDir == Direction::kLeft || CollisionDir == Direction::kRight) {
                        _Ball->SetVelocityX(-_Ball->GetVelocity().x);

                        GLfloat Penetration = _Ball->GetRadius() - glm::abs(DiffVector.x);
                        if (CollisionDir == Direction::kLeft) {
                            _Ball->SetPositionX(_Ball->GetPosition().x + Penetration);
                        } else {
                            _Ball->SetPositionX(_Ball->GetPosition().x - Penetration);
                        }
                    } else {
                        _Ball->SetVelocityY(-_Ball->GetVelocity().y);

                        GLfloat Penetration = _Ball->GetRadius() - glm::abs(DiffVector.y);
                        if (CollisionDir == Direction::kDown) {
                            _Ball->SetPositionY(_Ball->GetPosition().y + Penetration);
                        } else {
                            _Ball->SetPositionY(_Ball->GetPosition().y - Penetration);
                        }
                    }
                }
            }
        }
    }

    for (auto PowerUp : _PowerUps) {
        if (!PowerUp->GetDestructionState()) {
            if (PowerUp->GetPosition().y >= _WindowHeight) {
                PowerUp->SetDestructionState(GL_TRUE);
            }

            if (CheckCollision(PowerUp, _Paddle)) {
                ActivatePowerUp(PowerUp);
                PowerUp->SetActiveState(GL_TRUE);
                PowerUp->SetDestructionState(GL_TRUE);
                _SoundEngine->play2D(GetAssetFilepath(AssetType::kSound, "PowerUp.wav").c_str());
            }
        }
    }

    Collision BallPaddle = CheckCollision(_Ball, _Paddle);
    if (!_Ball->GetStuckState() && std::get<0>(BallPaddle)) {
        GLfloat   PaddleCenter = _Paddle->GetPosition().x + _Paddle->GetSize().x / 2.0f;
        GLfloat   Distance     = _Ball->GetPosition().x + _Ball->GetRadius() - PaddleCenter;
        GLfloat   Percentage   = Distance / (_Paddle->GetSize().x / 2.0f);
        GLfloat   Strength     = 2.0f;
        glm::vec2 PrevVelocity = _Ball->GetVelocity();

        _Ball->SetVelocityX(kBallVelocity.x * Percentage * Strength);
        _Ball->SetVelocityY(-1 * glm::abs(_Ball->GetVelocity().y));
        _Ball->SetVelocity(glm::normalize(_Ball->GetVelocity()) * glm::length(PrevVelocity));
        _Ball->SetStuckState(_Ball->GetStickyState());
        _SoundEngine->play2D(GetAssetFilepath(AssetType::kSound, "Bleep.wav").c_str());
    }
}

GLboolean Game::ShouldSpawnPowerUp(GLint Chance) {
    std::mt19937 RandomEngine;
    RandomEngine.seed(std::random_device{}());
    std::uniform_real_distribution<GLfloat> RandomGenerator(0.0f, static_cast<GLfloat>(std::numeric_limits<GLint>::max()));

    GLint  Random = static_cast<GLint>(RandomGenerator(RandomEngine)) % Chance;
    return Random == 0;
}

GLvoid Game::SpawnPowerUps(const GameObject* Brick) {
    if (ShouldSpawnPowerUp(_NegPowerUpChance)) {
        _PowerUps.emplace_back(new PowerUp(Brick->GetPosition(), _Assets->GetTexture("Chaos"),
                                           glm::vec3(0.9f, 0.25f, 0.25f), PowerUpType::kChaos, 10.0f));
    } else if (ShouldSpawnPowerUp(_NegPowerUpChance)) {
        _PowerUps.emplace_back(new PowerUp(Brick->GetPosition(), _Assets->GetTexture("Confuse"),
                                           glm::vec3(0.5f, 1.0f, 0.5f), PowerUpType::kConfuse, 10.0f));
    } else if (ShouldSpawnPowerUp(_PosPowerUpChance)) {
        _PowerUps.emplace_back(new PowerUp(Brick->GetPosition(), _Assets->GetTexture("PadSizeIncrease"),
                                           glm::vec3(1.0f, 0.6f, 0.4), PowerUpType::kPadSizeIncrease, 5.0f));
    } else if (ShouldSpawnPowerUp(_PosPowerUpChance)) {
        _PowerUps.emplace_back(new PowerUp(Brick->GetPosition(), _Assets->GetTexture("PassThrough"),
                                           glm::vec3(1.0f, 0.3f, 0.3f), PowerUpType::kPassThrough, 10.0f));
    } else if (ShouldSpawnPowerUp(_PosPowerUpChance)) {
        _PowerUps.emplace_back(new PowerUp(Brick->GetPosition(), _Assets->GetTexture("Speed"),
                                           glm::vec3(0.5f, 0.5f, 1.0f), PowerUpType::kSpeed, 5.0f));
    } else if (ShouldSpawnPowerUp(_PosPowerUpChance)) {
        _PowerUps.emplace_back(new PowerUp(Brick->GetPosition(), _Assets->GetTexture("Sticky"),
                                           glm::vec3(1.0f, 0.5f, 1.0f), PowerUpType::kSticky, 0.0f));
    }
}

GLvoid Game::ActivatePowerUp(const PowerUp* CollidedPowerUp) {
    switch (CollidedPowerUp->GetPowerUpType()) {
    case PowerUpType::kChaos: {
        _PostEffect->SetEffectState(Effects::kChaos, GL_TRUE);
        _PosPowerUpChance = 20;
        _NegPowerUpChance = 5;
        break;
    }
    case PowerUpType::kConfuse: {
        _PostEffect->SetEffectState(Effects::kConfuse, GL_TRUE);
        _PosPowerUpChance = 20;
        _NegPowerUpChance = 5;
        break;
    }
    case PowerUpType::kPadSizeIncrease: {
        _Paddle->SetSizeX(_Paddle->GetSize().x * 1.2f);
        break;
    }
    case PowerUpType::kPassThrough: {
        _Ball->SetPassThroughState(GL_TRUE);
        _Ball->SetColor({ 1.0f, 0.5f, 0.5f });
        _Particle->SetBurnState(GL_TRUE);
        break;
    }
    case PowerUpType::kSpeed: {
        _Paddle->SetVelocity({ _Paddle->GetVelocity() * 1.5f });
        break;
    }
    case PowerUpType::kSticky: {
        _Ball->SetStickyState(GL_TRUE);
        _StickyChance += 3;
        break;
    }
    }
}

GLvoid Game::UpdatePowerUps(GLfloat DeltaTime) {
    if (_StickyChance == 0) {
        _Ball->SetStickyState(GL_FALSE);
    }

    for (auto PowerUp : _PowerUps) {
        PowerUp->SetPosition(PowerUp->GetPosition() + PowerUp->GetVelocity() * DeltaTime);
        if (PowerUp->GetActiveState()) {
            PowerUp->SetDuration(PowerUp->GetDuration() - DeltaTime);
            if (PowerUp->GetDuration() <= 0.0f) {
                PowerUp->SetActiveState(GL_FALSE);
                if (PowerUp->GetPowerUpType() == PowerUpType::kChaos) {
                    if (!IsOtherPowerUpActivated(PowerUpType::kChaos)) {
                        _PostEffect->SetEffectState(Effects::kChaos, GL_FALSE);
                        _PosPowerUpChance = 50;
                        _NegPowerUpChance = 25;
                    }
                } else if (PowerUp->GetPowerUpType() == PowerUpType::kConfuse) {
                    if (!IsOtherPowerUpActivated(PowerUpType::kConfuse)) {
                        _PostEffect->SetEffectState(Effects::kConfuse, GL_FALSE);
                        _PosPowerUpChance = 50;
                        _NegPowerUpChance = 25;
                    }
                } else if (PowerUp->GetPowerUpType() == PowerUpType::kPadSizeIncrease) {
                    if (!IsOtherPowerUpActivated(PowerUpType::kPadSizeIncrease)) {
                        _Paddle->SetSize(kPaddleSize);
                    }
                } else if (PowerUp->GetPowerUpType() == PowerUpType::kPassThrough) {
                    if (!IsOtherPowerUpActivated(PowerUpType::kPassThrough)) {
                        _Ball->SetPassThroughState(GL_FALSE);
                        _Ball->SetColor(glm::vec3(1.0f));
                        _Particle->SetBurnState(GL_FALSE);
                    }
                } else if (PowerUp->GetPowerUpType() == PowerUpType::kSpeed) {
                    if (!IsOtherPowerUpActivated(PowerUpType::kSpeed)) {
                        _Paddle->SetVelocity(kPaddleVelocity);
                    }
                }
            }
        }
    }

    _PowerUps.erase(std::remove_if(_PowerUps.begin(), _PowerUps.end(),
                                   [](const auto* PowerUp) -> GLboolean {
                                       return PowerUp->GetDestructionState() & !PowerUp->GetActiveState();
                                   }), _PowerUps.end());;
}

GLboolean Game::IsOtherPowerUpActivated(PowerUpType Type) {
    for (const auto PowerUp : _PowerUps) {
        if (PowerUp->GetActiveState() && PowerUp->GetPowerUpType() == Type) {
            return GL_TRUE;
        }
    }

    return GL_FALSE;
}

GLvoid Game::ResetLevel() {
    for (auto& Brick : _Levels[_LevelIndex]->GetBricks()) {
        Brick->SetDestructionState(GL_FALSE);
    }

    _State = GameState::kMenu;
    _Lives = 3;

    for (GLint i = 0; i != 3; ++i) {
        _PostEffect->SetEffectState(static_cast<Effects>(i), GL_FALSE);
    }
}

GLvoid Game::ResetPlayer() {
    _Paddle->SetPosition(glm::vec2(_WindowWidth / 2.0f - kPaddleSize.x / 2.0f, _WindowHeight - kPaddleSize.y));
    _Ball->Reset(_Paddle->GetPosition() + glm::vec2(kPaddleSize.x / 2.0f - kBallRadius, -kBallRadius * 2.0f), kBallVelocity);
    ClearAllPositivePowerUps();
}

GLvoid Game::ClearAllPositivePowerUps() {
    _PowerUps.erase(std::remove_if(_PowerUps.begin(), _PowerUps.end(),
                                   [](const auto* PowerUp) -> GLboolean {
                                       return PowerUp->GetPowerUpType() != PowerUpType::kChaos &&
                                              PowerUp->GetPowerUpType() != PowerUpType::kConfuse;
                                   }), _PowerUps.end());;

    _Ball->SetColor(glm::vec3(1.0f));
    _Ball->SetPassThroughState(GL_FALSE);
    _Ball->SetStickyState(GL_FALSE);
    _Paddle->SetSize(kPaddleSize);
    _Paddle->SetVelocity(kPaddleVelocity);
    _Particle->SetBurnState(GL_FALSE);
}

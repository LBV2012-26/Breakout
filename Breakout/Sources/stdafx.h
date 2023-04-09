#pragma once
#ifndef STDAFX_H_
#define STDAFX_H_

#include <array>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>
#include <stb_image.h>

#include "AssetLoader/AssetManager.h"
#include "AssetLoader/GetAssetFilepath.h"
#include "AssetLoader/Shader.h"
#include "AssetLoader/Texture.h"
#include "Game/GameObject/BallObject.h"
#include "Game/GameObject/GameObject.h"
#include "Game/GameObject/PowerUp.hpp"
#include "Game/Renderer/PostProcessor.h"
#include "Game/Renderer/Sprite.h"
#include "Game/Game.h"
#include "Game/GameLevel.h"

#endif // !STDAFX_H_

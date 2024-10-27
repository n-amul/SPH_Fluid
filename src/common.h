#ifndef __COMMON_H__
#define __COMMON_H__

#include <filesystem>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>

using namespace std;
#define PI 3.14159265f

// typedef
#define CLASS_PTR(klassName)                                                                                           \
    class klassName;                                                                                                   \
    using klassName##UPtr = unique_ptr<klassName>;                                                                     \
    using klassName##Ptr = shared_ptr<klassName>;                                                                      \
    using klassName##WPtr = weak_ptr<klassName>;

optional<string> LoadTextFile(const string &filename);
glm::vec3 GetAttenuationCoeff(float distance);

#endif
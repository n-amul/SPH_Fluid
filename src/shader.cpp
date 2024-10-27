#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const string &filename, uint32_t shaderType)
{
    auto shader = ShaderUPtr(new Shader());
    if (!shader->loadFile(filename, shaderType))
    {
        return nullptr;
    }
    return std::move(shader);
}
Shader::~Shader()
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
}

bool Shader::loadFile(const string &filename, uint32_t shaderType)
{
    auto result = LoadTextFile(filename);
    if (!result.has_value())
    {
        return false;
    }

    auto &code = result.value();
    const char *codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length();

    // create and compile shader
    m_shader = glCreateShader(shaderType);
    glShaderSource(m_shader, 1, (const char *const *)&codePtr, &codeLength);
    glCompileShader(m_shader);

    // check compile error
    int sucess = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &sucess);
    if (!sucess)
    {
        char infolog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infolog);
        SPDLOG_ERROR("failed to compile shader: {}", filename);
        SPDLOG_ERROR("reason: {}", infolog);
        return false;
    }
    return true;
}
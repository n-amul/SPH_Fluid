#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"
// typedef for smart pointers
CLASS_PTR(Shader);
class Shader
{
public:
    static ShaderUPtr CreateFromFile(const string &filename, uint32_t shaderType);
    ~Shader();
    uint32_t Get() const
    {
        return m_shader;
    }

private:
    Shader() {}
    bool loadFile(const string &filename, uint32_t shaderType);
    uint32_t m_shader{0}; // shader ID
};

#endif // __SHADER_H__
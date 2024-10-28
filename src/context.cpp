#include "context.h"
#include "image.h"
#include <imgui.h>

Context::~Context()
{
    delete m_sphSystem;
}

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
    {
        return nullptr;
    }
    return move(context);
}
void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);

    m_framebuffer = Framebuffer::Create(Texture::Create(width, height, GL_RGBA));
}
void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}
void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}
bool Context::Init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    // Log the current working directory
    std::filesystem::path current_path = std::filesystem::current_path();
    SPDLOG_INFO("Current working directory: {}", current_path.string());
    
    m_simpleProgram = Program::Create("../../shader/simple.vs", "../../shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    m_program = Program::Create("../../shader/lighting.vs", "../../shader/lighting.fs");
    if (!m_program)
        return false;
    
    m_particles = Program::Create("../../shader/particle.vs", "../../shader/particle.fs");
    if(!m_particles)
        return false;

    //load objs
    m_lightbox=Mesh::CreateBox();
    SPHSettings sphSettings(0.02f, 1000, 1, 1.04f, 0.15f, -9.8f, 0.2f);
    m_sphSystem = new SphSystem(15, sphSettings, false);

    return true;
}
void Context::Render()
{
    if (ImGui::Begin("ui window")) {
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);

    Framebuffer::BindToDefault();

    m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
                    glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    auto projection = glm::perspective(glm::radians(45.0f), (float)(m_width / m_height), 0.01f, 100.0f);
    //sph system
    m_sphSystem->update(0.003f);
    m_sphSystem->draw(projection*view,m_particles.get());
}

void Context::ProcessInput(GLFWwindow *window)
{
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
    if(glfwGetKey(window,GLFW_KEY_C)==GLFW_PRESS){
        m_sphSystem->startSimulation();
    }
    if(glfwGetKey(window,GLFW_KEY_R)==GLFW_PRESS){
        m_sphSystem->reset();
    }
}
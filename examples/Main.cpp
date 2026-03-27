#include <pch.h>
#include "App.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <iostream>
#include <ostream>

// Glew debug callback
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH)
        std::cout << "GL ERROR: " << msg << std::endl;
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
        std::cout << "GL WARNING: " << msg << std::endl;
}

void OnGLFWwindowResize(GLFWwindow* window, int width, int height)
{
    OnWindowResize(width, height);
}

void OnGLFWwindowRefresh(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    AppUpdate(0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Sandbox", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);

    /* Disable Vsync */
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, OnGLFWwindowResize);
    glfwSetWindowRefreshCallback(window, OnGLFWwindowRefresh);

    std::cout << glGetString(GL_VERSION) << std::endl;

    glewInit();

    // Debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);

    AppStart();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    ImGui::StyleColorsDark();

    double time = glfwGetTime();
    double lastTime = time;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        AppUpdate(time - lastTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        lastTime = time;
    }

    AppStop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
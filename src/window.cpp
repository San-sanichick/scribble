#include "window.hpp"


inline void resizeHandler(GLFWwindow* window, int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
}


scribble::Window::Window(std::string_view name)
    : _name(name)
{ }

void scribble::Window::init()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    this->_width = mode->width;
    this->_height = mode->height;

    this->_window = glfwCreateWindow(
        this->_width,
        this->_height,
        this->_name.data(),
        nullptr,
        nullptr
    );

    glfwMakeContextCurrent(this->_window);

    glfwSetFramebufferSizeCallback(
        this->_window,
        resizeHandler
    );
}

scribble::Window::~Window()
{
    glfwDestroyWindow(this->_window);
}

bool scribble::Window::shouldClose() const
{
    return glfwWindowShouldClose(this->_window) != 0;
}

uint32_t scribble::Window::width()
{
    int32_t w;
    glfwGetWindowSize(this->_window, &w, nullptr);
    this->_width = w;
    return this->_width;
}

uint32_t scribble::Window::height()
{
    int32_t h;
    glfwGetWindowSize(this->_window, nullptr, &h);
    this->_height = h;
    return this->_height;
}

float scribble::Window::aspectRatio()
{
    int32_t w, h;
    glfwGetWindowSize(this->_window, &w, &h);

    this->_width = w;
    this->_height = h;

    return static_cast<float>(w) / static_cast<float>(h);
}

GLFWwindow* scribble::Window::id() const
{
    return this->_window;
}


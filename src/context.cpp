#include "context.hpp"


scribble::Context::Context(const Window& window)
    : window(window)
{ }


bool scribble::Context::init() const
{
    glfwMakeContextCurrent(this->window.id());
    int32_t status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    CORE_ASSERT(status, "Failed to init GLAD");

    CORE_LOGN("Info:");
    CORE_LOGN("  Vendor: " << glGetString(GL_VENDOR));
    CORE_LOGN("  Renderer: " << glGetString(GL_RENDERER));
    CORE_LOGN("  Version: " << glGetString(GL_VERSION));

    return true;
}

void scribble::Context::swapBuffers() const
{
    glfwSwapBuffers(this->window.id());
}

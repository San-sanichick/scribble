#include "pch.hpp"
#include "window.hpp"
#include "context.hpp"
#include <GLFW/glfw3.h>


int main()
{
    CORE_LOGN("GL init");
    glfwInit();

    scribble::Window window("Scribble");
    scribble::Context ctx(window);
    window.init();
    ctx.init();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);

    while (!window.shouldClose())
    {
        glfwPollEvents();
        if (glfwGetKey(window.id(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window.id(), true);
        }

        {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        ctx.swapBuffers();
    }

    glfwTerminate();
}

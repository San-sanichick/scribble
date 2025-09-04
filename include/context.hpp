#pragma once

#include "window.hpp"


namespace scribble
{

class Context
{
public:
    Context(const Window& window);

    bool init() const;
    void swapBuffers() const;

private:
    const Window& window;
};

}


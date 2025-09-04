#pragma once

#include "pch.hpp"


namespace scribble
{

class Window
{
    public:
        Window(std::string_view name);
        ~Window();

        void init();
        bool shouldClose() const;

        u32 width();
        u32 height();

        f32 aspectRatio();
        
        GLFWwindow* id() const;


    private:
        u32 _width {};
        u32 _height {};

        std::string_view _name;

        GLFWwindow* _window;
};

}

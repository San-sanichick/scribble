#pragma once

#include "pch.hpp"
#include "raylib.h"
#include <vector>


namespace scribble
{

enum class SHAPE_TYPE
{
    NONE = 0,
    LINE,
    PEN,
    RECT,
    // ELLIPSE,
    ARROW,
    TEXT,
};


struct Shape
{
    SHAPE_TYPE type = SHAPE_TYPE::NONE;
    u16 x1 {};
    u16 y1 {};
    u16 x2 {};
    u16 y2 {};
    f32 thickness {};
    Color color;
    char* text = nullptr;
    Vector2* points = nullptr;
    u16 pointCount {};
};

class PaintMachine
{
public:
    PaintMachine() = default;

    void add(Shape* shape) { this->_shapes.push_back(shape); }
    void removeTop()
    {
        if (this->_shapes.size() > 0)
            this->_shapes.pop_back();
    }

    const std::vector<Shape*>& shapes() { return this->_shapes; }

private:
    std::vector<Shape*> _shapes;
};

}

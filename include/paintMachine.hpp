#pragma once

#include "pch.hpp"
#include "raylib.h"
#include <complex>
#include <optional>
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


// I could use ECS instead of std::optional.
// Or I could not :)
struct Shape
{
    SHAPE_TYPE type = SHAPE_TYPE::NONE;

    u16 x1 {};
    u16 y1 {};
    u16 x2 {};
    u16 y2 {};

    f32 thickness {};
    Color color;

    std::optional<std::string> text;
    std::optional<std::vector<Vector2>> points;
};

class PaintMachine
{
public:
    PaintMachine() = default;

    void add(std::unique_ptr<Shape>& shape)
    {
        this->_shapes.emplace_back(std::move(shape));
    }

    void removeTop()
    {
        if (this->_shapes.size() == 0) return;

        this->_shapes.pop_back();
    }

    const std::vector<std::unique_ptr<Shape>>& shapes()
    {
        return this->_shapes;
    }

private:
    std::vector<std::unique_ptr<Shape>> _shapes;
};

}

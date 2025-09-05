#include "pch.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <raylib.h>
#include "paintMachine.hpp"


#include "raylib.h"
#include "raymath.h"

void DrawArrow(Vector2 start, Vector2 end, float thickness, Color color) {
    // Draw the main shaft
    DrawLineEx(start, end, thickness, color);

    // Calculate direction and length
    Vector2 dir = Vector2Subtract(end, start);
    float len = Vector2Length(dir);
    if (len < 0.001f) return;  // Skip if too short to draw

    Vector2 norm = Vector2Normalize(dir);

    // Arrowhead parameters (adjust to taste)
    const float arrowLength = thickness * 3.0f;  // Length of each barb
    const float arrowAngle = 40.0f;  // Angle of barbs in degrees (e.g., 30 for a standard arrow)

    // Calculate barb directions (rotate the reversed normal)
    Vector2 reverseNorm = Vector2Scale(norm, -arrowLength);
    Vector2 barb1 = Vector2Rotate(reverseNorm, arrowAngle);
    Vector2 barb2 = Vector2Rotate(reverseNorm, -arrowAngle);

    // Calculate barb endpoints
    Vector2 barb1Pos = Vector2Add(start, barb1);
    Vector2 barb2Pos = Vector2Add(start, barb2);

    // Draw the barbs
    DrawLineEx(start, barb1Pos, thickness, color);
    DrawLineEx(start, barb2Pos, thickness, color);
    DrawCircle(start.x, start.y, thickness * 0.5f, color);
}

void shapeRenderer(const scribble::Shape* shape, f32 thickness, Color& color)
{
    switch (shape->type)
    {
    case scribble::SHAPE_TYPE::NONE:
        break;
    case scribble::SHAPE_TYPE::LINE:
        DrawLineEx(
            {
                static_cast<float>(shape->x1),
                static_cast<float>(shape->y1),
            },
            {
                static_cast<float>(shape->x2),
                static_cast<float>(shape->y2),
            },
            thickness,
            color
        );
        break;
    case scribble::SHAPE_TYPE::PEN:
        DrawSplineCatmullRom(shape->points, shape->pointCount, thickness, color);
        break;
    case scribble::SHAPE_TYPE::RECT:
    {
        float minX = std::min(shape->x1, shape->x2);
        float minY = std::min(shape->y1, shape->y2);
        float maxX = std::max(shape->x1, shape->x2);
        float maxY = std::max(shape->y1, shape->y2);

        DrawRectangleLinesEx(
            {
                minX,
                minY,
                maxX - minX,
                maxY - minY,
            },
            thickness,
            color
        );
        break;
    }
    case scribble::SHAPE_TYPE::ARROW:
        DrawArrow(
            {
                static_cast<float>(shape->x1),
                static_cast<float>(shape->y1),
            },
            {
                static_cast<float>(shape->x2),
                static_cast<float>(shape->y2),
            },
            thickness,
            color
        );
        break;
    case scribble::SHAPE_TYPE::TEXT:
      break;
    }
}


int main()
{
    // borderless fullscreen
    const i32 sWidth = 0;
    const i32 sHeight = 0;

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(sWidth, sHeight, "Scribble");
    SetWindowState(FLAG_WINDOW_UNDECORATED);

    SetTargetFPS(60);

    scribble::PaintMachine machine;
    Color color = PINK;

    bool isMouseDown = false;
    scribble::SHAPE_TYPE currentType = scribble::SHAPE_TYPE::LINE;
    scribble::Shape* currentShape = nullptr;

    f32 thickness = 15.0f;
    std::vector<Vector2> points;
    while (!WindowShouldClose())
    {
        auto pos = GetMousePosition();
        thickness += (i32)GetMouseWheelMove() * 1;


        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C))
        {
            Image img = LoadImageFromScreen();

            i32 size;
            u8* pngData = ExportImageToMemory(img, ".png", &size);
            CORE_ASSERT(pngData && size != 0, "Failed to export");

            FILE* pipe = popen("wl-copy --type image/png", "w");
            CORE_ASSERT(pipe, "Failed to create a pipe");

            size_t written = fwrite(pngData, 1, size, pipe);
            CORE_ASSERT(written == (size_t)size, "Failed to write correctly");

            pclose(pipe);
            MemFree(pngData);
        }

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_Z))
        {
            machine.removeTop();
        }

        if (!isMouseDown && IsKeyPressed(KEY_R))
        {
            color = RED;
        }

        if (!isMouseDown && IsKeyPressed(KEY_P))
        {
            color = PINK;
        }

        if (!isMouseDown && IsKeyPressed(KEY_G))
        {
            color = GREEN;
        }

        if (!isMouseDown && IsKeyPressed(KEY_B))
        {
            color = BLUE;
        }

        currentType = scribble::SHAPE_TYPE::PEN;

        if (
            (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) &&
            (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        )
        {
            currentType = scribble::SHAPE_TYPE::ARROW;
        }
        else if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
        {
            currentType = scribble::SHAPE_TYPE::RECT;
        }
        else if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        {
            currentType = scribble::SHAPE_TYPE::LINE;
        }


        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !isMouseDown)
        {
            if (currentShape == nullptr)
                currentShape = new scribble::Shape();

            isMouseDown = true;
            currentShape->type = currentType;
            currentShape->color = color;
            currentShape->x1 = pos.x;
            currentShape->y1 = pos.y;
            currentShape->thickness = thickness;
        }

        if (isMouseDown && currentShape != nullptr)
        {
            currentShape->x2 = pos.x;
            currentShape->y2 = pos.y;

            if (currentShape->type == scribble::SHAPE_TYPE::PEN)
            {
                points.push_back(pos);
            }

            currentShape->points = new Vector2[points.size()];
            currentShape->pointCount = points.size();
            std::memcpy(currentShape->points, points.data(), points.size() * sizeof(Vector2));
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
        {
            isMouseDown = false;
            if (currentShape != nullptr)
            {
                machine.add(currentShape);
                currentShape = nullptr;
            }

            if (currentType == scribble::SHAPE_TYPE::PEN)
            {
                points.clear();
            }
        }

        BeginDrawing();
        {
            ClearBackground(BLANK);
            DrawCircle(pos.x, pos.y, thickness * 0.5f, color);
            
            if (currentShape)
            {
                shapeRenderer(currentShape, thickness, color);
            }

            for (const auto& shape : machine.shapes())
            {
                shapeRenderer(shape, shape->thickness, shape->color);
            }
        }
        EndDrawing();
    }

    CloseWindow();
}

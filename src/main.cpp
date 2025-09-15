#include "pch.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <raylib.h>
#include <string_view>
#include "paintMachine.hpp"


#include "raylib.h"
#include "raymath.h"
#include "utils/debug.hpp"

void DrawArrow(Vector2 start, Vector2 end, f32 thickness, Color color)
{
    DrawLineEx(start, end, thickness, color);

    Vector2 dir = Vector2Subtract(end, start);
    f32 len = Vector2Length(dir);
    if (len < 0.001f) return;

    Vector2 norm = Vector2Normalize(dir);

    const f32 arrowLength = thickness * 3.0f; 
    const f32 arrowAngle = 40.0f;

    Vector2 reverseNorm = Vector2Scale(norm, -arrowLength);
    Vector2 barb1 = Vector2Rotate(reverseNorm, arrowAngle);
    Vector2 barb2 = Vector2Rotate(reverseNorm, -arrowAngle);

    Vector2 barb1Pos = Vector2Add(start, barb1);
    Vector2 barb2Pos = Vector2Add(start, barb2);

    DrawLineEx(start, barb1Pos, thickness, color);
    DrawLineEx(start, barb2Pos, thickness, color);
    DrawCircle(start.x, start.y, thickness * 0.5f, color);
    DrawCircle(barb1Pos.x, barb1Pos.y, thickness * 0.5f, color);
    DrawCircle(barb2Pos.x, barb2Pos.y, thickness * 0.5f, color);
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
                static_cast<f32>(shape->x1),
                static_cast<f32>(shape->y1),
            },
            {
                static_cast<f32>(shape->x2),
                static_cast<f32>(shape->y2),
            },
            thickness,
            color
        );
        break;
    case scribble::SHAPE_TYPE::PEN:
        if (shape->points.has_value())
            DrawSplineCatmullRom(shape->points->data(), shape->points->size(), thickness, color);

        break;
    case scribble::SHAPE_TYPE::RECT:
    {
        f32 minX = std::min(shape->x1, shape->x2);
        f32 minY = std::min(shape->y1, shape->y2);
        f32 maxX = std::max(shape->x1, shape->x2);
        f32 maxY = std::max(shape->y1, shape->y2);

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
                static_cast<f32>(shape->x1),
                static_cast<f32>(shape->y1),
            },
            {
                static_cast<f32>(shape->x2),
                static_cast<f32>(shape->y2),
            },
            thickness,
            color
        );
        break;
    case scribble::SHAPE_TYPE::TEXT:
        if (shape->text.has_value())
        {
            DrawText(
                shape->text->c_str(),
                shape->x1,
                shape->y1 - thickness,
                thickness * 2.0f,
                color
            );
        }
        break;
    }
}

Vector2 getCaretPos(const std::string& text, f32 fontSize, const Vector2& pos)
{
    i32 lines = 0;
    size_t lastNewline = 0;
    f32 width = 0.0f;
    f32 height = 0.0f;
    
    for (size_t i = 0; i < text.size(); i++)
    {
        u8 c = text[i];
        if (c == '\n')
        {
            lines++;
            lastNewline = i + 1;
        }

        if (i == text.size() - 1)
        {
            size_t size = i - lastNewline + 1;
            std::string_view slice(text.c_str() + lastNewline, size);

            width = MeasureText(slice.data(), fontSize * 2.0f);
        }
    }

    height = (fontSize * 2.0f) * lines;

    return {
        pos.x + width + (width > 0.0f ? (fontSize * 0.5f) : 0.0f),
        pos.y + height,
    };
}

void saveToClipboard()
{
    Image img = LoadImageFromScreen();

    i32 size;
    u8* pngData = ExportImageToMemory(img, ".png", &size);
    CORE_ASSERT(pngData && size != 0, "Failed to export");

    FILE* pipe = popen("wl-copy --type image/png", "w");
    CORE_ASSERT(pipe, "Failed to create a pipe");

#ifdef DEBUG_BUILD
    size_t written = fwrite(pngData, 1, size, pipe);
    CORE_ASSERT(written == (size_t)size, "Failed to write correctly");
#else
    fwrite(pngData, 1, size, pipe);
#endif

    pclose(pipe);
    MemFree(pngData);
}

// shall the rust cult burn in hell
int main()
{
    // borderless fullscreen
    const i32 sWidth = 0;
    const i32 sHeight = 0;

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(sWidth, sHeight, "Scribble");
    SetWindowState(FLAG_WINDOW_UNDECORATED);

    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    scribble::PaintMachine machine;
    Color color = PINK;

    std::unique_ptr<scribble::Shape> currentShape = nullptr;

    bool isMouseDown = false;
    scribble::SHAPE_TYPE currentType = scribble::SHAPE_TYPE::LINE;

    f32 thickness = 15.0f;
    bool textDone = true;

    while (!WindowShouldClose())
    {
        auto pos = GetMousePosition();
        thickness += (i32)GetMouseWheelMove() * 1;


        if (
            (IsKeyDown(KEY_LEFT_CONTROL) ||
             IsKeyDown(KEY_RIGHT_CONTROL)) &&
            IsKeyPressed(KEY_C)
        )
        {
            saveToClipboard();
        }

        if (
            (IsKeyDown(KEY_LEFT_CONTROL) ||
             IsKeyDown(KEY_RIGHT_CONTROL)) &&
            IsKeyPressed(KEY_Z)
        )
        {
            machine.removeTop();
        }

        if (!isMouseDown && textDone && IsKeyPressed(KEY_R))
        {
            color = RED;
        }

        if (!isMouseDown && textDone && IsKeyPressed(KEY_P))
        {
            color = PINK;
        }

        if (!isMouseDown && textDone && IsKeyPressed(KEY_G))
        {
            color = GREEN;
        }

        if (!isMouseDown && textDone && IsKeyPressed(KEY_B))
        {
            color = BLUE;
        }

        if (currentType != scribble::SHAPE_TYPE::TEXT)
        {
            currentType = scribble::SHAPE_TYPE::PEN;
        }

        if (IsKeyPressed(KEY_T))
        {
            currentType = scribble::SHAPE_TYPE::TEXT;
        }
        else if (
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
            if (!currentShape)
                currentShape = std::make_unique<scribble::Shape>();

            if (currentType == scribble::SHAPE_TYPE::PEN)
                currentShape->points = std::vector<Vector2>();

            isMouseDown = true;
            currentShape->type = currentType;
            currentShape->color = color;
            currentShape->x1 = pos.x;
            currentShape->y1 = pos.y;
            currentShape->thickness = thickness;

            if (currentShape->type == scribble::SHAPE_TYPE::TEXT)
            {
                currentShape->text = std::string();
                textDone = false;
            }
        }

        if (!isMouseDown && !textDone)
        {
            i32 key = GetCharPressed();

            while (key > 0)
            {
                currentShape->text->push_back(static_cast<u8>(key));
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (currentShape->text->size() > 0)
                    currentShape->text->pop_back();
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                currentShape->text->push_back('\n');
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (!textDone)
            {
                textDone = true;
                currentType = scribble::SHAPE_TYPE::PEN;
            }
            else
            {
                CloseWindow();
                return 0;
            }
        }

        if (isMouseDown && currentShape)
        {
            currentShape->x2 = pos.x;
            currentShape->y2 = pos.y;

            if (
                currentShape->type == scribble::SHAPE_TYPE::PEN &&
                currentShape->points.has_value()
            )
            {
                currentShape->points->push_back(pos);
            }
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
        {
            isMouseDown = false;
            if (currentShape && textDone)
            {
                machine.add(currentShape);
                currentShape = nullptr;
            }
        }

        BeginDrawing();
        {
            ClearBackground(BLANK);

            if (currentShape == nullptr)
            {
                if (currentType == scribble::SHAPE_TYPE::TEXT)
                {
                    DrawLineEx(
                        { pos.x, pos.y + thickness },
                        { pos.x, pos.y - thickness },
                        thickness * 0.5f,
                        color
                    );
                }
                else
                {
                    DrawCircle(pos.x, pos.y, thickness * 0.5f, color);
                }
            }

            for (const auto& shape : machine.shapes())
            {
                shapeRenderer(shape.get(), shape->thickness, shape->color);
            }
            
            if (currentShape)
            {
                if (currentShape->type == scribble::SHAPE_TYPE::TEXT)
                {
                    auto caretPos = getCaretPos(
                        currentShape->text.value(),
                        thickness,
                        { static_cast<f32>(currentShape->x1), static_cast<f32>(currentShape->y1) }
                    );

                    DrawLineEx(
                        { caretPos.x, caretPos.y + thickness },
                        { caretPos.x, caretPos.y - thickness },
                        thickness * 0.5f,
                        color
                    );
                }
                shapeRenderer(currentShape.get(), thickness, color);
            }
        }
        EndDrawing();
    }

    CloseWindow();
}

#include <cstdint>
#include <iostream>
#define CLAY_IMPLIMENTATION
#include "clay/clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"
int main() {
    std::cout << "Hello, World!" << std::endl;
    Clay_Raylib_Initialize(800, 600, "Hello Clay+Raylib", FLAG_WINDOW_RESIZABLE);
    uint64_t total_mem = Clay_MinMemorySize();
    Clay_Arena arena = (Clay_Arena) {
        .capacity = total_mem,
        .memory = (char*)malloc(total_mem)
    };
    Clay_Initialize(arena, (Clay_Dimensions) {
        .width = static_cast<float>(GetScreenWidth()),
        .height = static_cast<float>(GetScreenHeight())
    });

    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions({
            .width = static_cast<float>(GetScreenWidth()),
            .height = static_cast<float>(GetScreenHeight())
        });
        Clay_BeginLayout();
        CLAY(
            CLAY_RECTANGLE({.color = {255, 0, 0, 255}}),
            CLAY_LAYOUT({
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
                }
            })
        ) {}
        Clay_RenderCommandArray render_commands = Clay_EndLayout();
        BeginDrawing();
        Clay_Raylib_Render(render_commands);
        EndDrawing();
    }
}

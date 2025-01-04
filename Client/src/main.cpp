#include <cstdint>
#include <iostream>
#include <string>
#define CLAY_IMPLEMENTATION
#include "clay/clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"
void clay_error_printer_func(Clay_ErrorData data) {
    std::string text = std::string(data.errorText.chars);
    std::cout << "Error: " << text << std::endl;
}

// Create the error handler structure
Clay_ErrorHandler clay_error_printer = {
    .errorHandlerFunction = clay_error_printer_func,
    .userData = 0
};
int main() {
    std::cout << "Hello, World!" << std::endl;
    Clay_Raylib_Initialize(800, 600, "Hello Clay+Raylib", FLAG_WINDOW_RESIZABLE);
    uint64_t total_mem = Clay_MinMemorySize();
    Clay_Arena arena = (Clay_Arena) {
        .capacity = total_mem,
        .memory = (char*)malloc(total_mem)
    };
    Clay_Initialize(arena, {
        .width = 800,
        .height = 600
    }, clay_error_printer);
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

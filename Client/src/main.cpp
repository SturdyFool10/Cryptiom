#include "clay/renderers/raylib/raylib.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <chrono>
#define CLAY_IMPLEMENTATION
#include "clay/clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#include "config.hpp"


//files from this project below

using std::cout;
using std::string;

void clay_error_printer_func(Clay_ErrorData data) {
  string text = std::string(data.errorText.chars);
  cout << "Error: " << text << std::endl;
}

// Create the error handler structure
Clay_ErrorHandler clay_error_printer = {
    .errorHandlerFunction = clay_error_printer_func,
    .userData = 0
};
int main() {
    auto conf = Config::loadConfig("config.json");
    if (conf.is_err()) {
        cout << "Failed to load config: " << conf.unwrap_err() << std::endl;
        return 1;
    }
    auto config = conf.unwrap();
  Clay_Raylib_Initialize(
      config.window.width, config.window.height, "Hello Clay+Raylib",
      FLAG_WINDOW_RESIZABLE | (config.window.antiAliasing ? FLAG_MSAA_4X_HINT : 0) | (config.window.vsync ? FLAG_VSYNC_HINT : 0));
                              // TODO: switch backend to something that is fully
                              // feature-supported and can render while resizing
                              // is occurring. SDL2 is the only canidate from
                              // the default backends but it is not fully
                              // supported as of 1/4/2024
  uint64_t total_mem = Clay_MinMemorySize();
  Clay_Arena arena =
      (Clay_Arena){.capacity = total_mem, .memory = (char *)malloc(total_mem)};
  Clay_Initialize(arena, {.width = 800, .height = 600}, clay_error_printer);

  auto last_time = std::chrono::high_resolution_clock::now();
  float fps = 0.0f;
  auto last_fps_log_time = std::chrono::high_resolution_clock::now();
  while (!WindowShouldClose()) {
    auto current_time = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(current_time - last_time).count();
    fps = 1.0f / dt;
    last_time = current_time;
    auto current_fps_log_time = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration<float>(current_fps_log_time - last_fps_log_time).count() > 1.0f) {
        std::cout << "FPS: " << fps << std::endl;
        last_fps_log_time = current_fps_log_time;
    }
    Clay_SetLayoutDimensions({.width = static_cast<float>(GetScreenWidth()),
                              .height = static_cast<float>(GetScreenHeight())});
    Clay_BeginLayout();
    Clay_Color menu_bg_color = {55, 55, 61, 255};
    Clay_Sizing layout_expando = {.width = CLAY_SIZING_GROW(),
                                  .height = CLAY_SIZING_GROW()};
    Clay_RectangleElementConfig rectangle_config = {.color = menu_bg_color,
                                                    .cornerRadius = {20}};
    CLAY(CLAY_ID("Root"), CLAY_RECTANGLE({.color = {41, 41, 51, 255}}),
         CLAY_LAYOUT({.sizing = layout_expando,
                      .padding = {.x = 16, .y = 16},
                      .childGap = 5,
                      .layoutDirection = CLAY_TOP_TO_BOTTOM})) { // Root children -->

      CLAY(CLAY_ID("HeaderBar"), CLAY_RECTANGLE({rectangle_config}),
           CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_GROW(),
                                   .height = CLAY_SIZING_FIXED(60)},
                        .layoutDirection = CLAY_LEFT_TO_RIGHT})) { // HeaderBar children -->'

        // header children
      } // <-- HeaderBar children
      CLAY(CLAY_ID("LowerContent"),
           CLAY_LAYOUT({.sizing = layout_expando,
                        .childGap = 16,
                        .layoutDirection = CLAY_LEFT_TO_RIGHT})) { // LowerContent children -->

        CLAY(CLAY_ID("LeftBar"),
             CLAY_RECTANGLE({.color = menu_bg_color, .cornerRadius = {20}}),
             CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_FIXED(100),
                                     .height = CLAY_SIZING_GROW()},
                          .layoutDirection = CLAY_TOP_TO_BOTTOM})) { // LeftBar children -->
        } // <-- LeftBar children
        CLAY(CLAY_ID("MainContent"), CLAY_RECTANGLE({rectangle_config}),
             CLAY_LAYOUT({.sizing = layout_expando,
                          .layoutDirection = CLAY_TOP_TO_BOTTOM})) { // MainContent children -->
        } // <-- MainContent children
      } // <-- LowerContent children
    } // <-- Root children
    Clay_RenderCommandArray render_commands = Clay_EndLayout();
    BeginDrawing();
    ClearBackground(Color{41, 41, 51, 255});
    Clay_Raylib_Render(render_commands);
    EndDrawing();
    }
}

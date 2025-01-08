-- premake5.lua
require "export-compile-commands"

workspace "Client"
configurations { "Debug", "Release" }
language "C++"
cppdialect "C++23"
architecture "x86_64"
toolset "clang"
targetdir "bin/%{cfg.buildcfg}"
objdir "bin-int/%{cfg.buildcfg}"

filter "action:not vs*"
symbols "On"

project "Client"
kind "ConsoleApp"
location "build"

files {
    "src/**.c",
    "src/**.h",
    "src/**.cpp",
    "src/**.hpp"
}

includedirs {
    "deps/include",
    "C:/raylib/raylib/src"
}

libdirs {
    "deps/libs(Static)/"
}

links { "raylib.lib", "winmm.lib" }

filter "configurations:Debug"
defines { "DEBUG" }
runtime "Debug"
optimize "Off"

filter "configurations:Release"
defines { "RELEASE" }
runtime "Release"
optimize "Full"

-- Export compile commands for both Debug and Release configurations
if _ACTION == "export-compile-commands" then
    require "export-compile-commands"
end

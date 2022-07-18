#pragma once

#include <Windows.h>
#include <iostream>
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")

#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include "backend/imgui_impl_opengl2.h"

#include "TwoThreeTree.hpp"
#include "Log.hpp"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


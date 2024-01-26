#pragma once
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "globals.h"
#include "logger.h"
#include "utils.h"

#include "project.h"

#include "hacks/patterns.h"

#include <Geode/Geode.hpp>
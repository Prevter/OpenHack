#pragma once

#ifdef _WIN32
#define PLATFORM_WINDOWS
#include "win32/win32.hpp"
#elif defined(__APPLE__)
#define PLATFORM_MACOS
#error "MacOS is not supported yet"
#include "macos/macos.hpp"
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#error "Android is not supported yet"
#include "android/android.hpp"
#endif
#pragma once

#ifdef _WIN32
#define PLATFORM_WINDOWS
#define ON_WINDOWS(...) __VA_ARGS__
#define ON_MACOS(...)
#define ON_ANDROID(...)
#include "win32/win32.hpp"
#elif !defined(OPENHACK_GEODE)
#error "Standalone supports only Windows."
#elif defined(__APPLE__)
#define PLATFORM_MACOS
#define ON_WINDOWS(...)
#define ON_MACOS(...) __VA_ARGS__
#define ON_ANDROID(...)
#include "macos/macos.hpp"
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#define ON_WINDOWS(...)
#define ON_MACOS(...)
#define ON_ANDROID(...) __VA_ARGS__
#include "android/android.hpp"
#else
#error "Unsupported platform."
#endif

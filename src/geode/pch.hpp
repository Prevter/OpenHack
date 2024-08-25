#pragma once

#include "../shared/openhack.hpp"
#include "utils.hpp"

#include <Geode/Geode.hpp>

#define L_TRACE(...) geode::log::debug(__VA_ARGS__)
#define L_INFO(...) geode::log::info(__VA_ARGS__)
#define L_WARN(...) geode::log::warn(__VA_ARGS__)
#define L_ERROR(...) geode::log::error(__VA_ARGS__)
#define L_CRITICAL(...) geode::log::error(__VA_ARGS__)
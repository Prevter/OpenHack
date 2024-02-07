#pragma once
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>

namespace logger
{
    void initialize(bool console, bool file, std::string filename = "log.txt");
    void setConsole(bool enabled);
    spdlog::logger *getLogger();
}

#define L_TRACE(...) ::logger::getLogger()->trace(__VA_ARGS__)
#define L_INFO(...) ::logger::getLogger()->info(__VA_ARGS__)
#define L_WARN(...) ::logger::getLogger()->warn(__VA_ARGS__)
#define L_ERROR(...) ::logger::getLogger()->error(__VA_ARGS__)
#define L_CRITICAL(...) ::logger::getLogger()->critical(__VA_ARGS__)
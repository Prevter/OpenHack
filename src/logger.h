#pragma once
#include <string>
#include <stdint.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace logger
{
    void init(bool console, bool file, std::string file_name = "log.txt");

    spdlog::logger *get_logger();

    void set_exception_handler();
}

#define L_TRACE(...) ::logger::get_logger()->trace(__VA_ARGS__)
#define L_INFO(...) ::logger::get_logger()->info(__VA_ARGS__)
#define L_WARN(...) ::logger::get_logger()->warn(__VA_ARGS__)
#define L_ERROR(...) ::logger::get_logger()->error(__VA_ARGS__)
#define L_CRITICAL(...) ::logger::get_logger()->critical(__VA_ARGS__)
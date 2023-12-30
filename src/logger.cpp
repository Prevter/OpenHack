#include "logger.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <windows.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logger
{
    static std::ofstream log_file;
    static bool log_console = false;
    static bool log_file_enabled = false;
    static std::string log_file_name = "log.txt";

    static std::shared_ptr<spdlog::logger> logger;

    void init(bool console, bool file, std::string file_name)
    {
        std::vector<spdlog::sink_ptr> sinks;

        if (console)
        {
            AllocConsole();
            FILE *fDummy;
            freopen_s(&fDummy, "CONIN$", "r", stdin);

            sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
            sinks[0]->set_pattern("%^[%T] %n: %v%$");
        }

        if (file)
        {
            sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_name, true));
            sinks[console ? 1 : 0]->set_pattern("[%T] [%l] %n: %v");
        }

        logger = std::make_shared<spdlog::logger>("OpenHack", begin(sinks), end(sinks));
        spdlog::register_logger(logger);
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);
    }

    spdlog::logger *get_logger()
    {
        return logger.get();
    }

    void set_exception_handler()
    {
        SetUnhandledExceptionFilter([](EXCEPTION_POINTERS *exception_info) -> LONG
                                    {
        L_CRITICAL("Unhandled exception: 0x%X", exception_info->ExceptionRecord->ExceptionCode);
        L_CRITICAL("Exception address: 0x%X", exception_info->ExceptionRecord->ExceptionAddress);
        L_CRITICAL("Exception flags: 0x%X", exception_info->ExceptionRecord->ExceptionFlags);
        L_CRITICAL("Exception parameters: 0x%X", exception_info->ExceptionRecord->NumberParameters);
        for (size_t i = 0; i < exception_info->ExceptionRecord->NumberParameters; i++) {
            L_CRITICAL("Exception parameter #%d: 0x%X", i, exception_info->ExceptionRecord->ExceptionInformation[i]);
        }
        L_CRITICAL("Exception thread ID: 0x%X", exception_info->ExceptionRecord->ExceptionInformation[0]);
        L_CRITICAL("Exception thread handle: 0x%X", exception_info->ExceptionRecord->ExceptionInformation[1]);
        return EXCEPTION_EXECUTE_HANDLER; });
    }
}
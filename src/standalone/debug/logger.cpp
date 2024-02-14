#include "logger.hpp"
#include "../../shared/platform/platform.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <windows.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/xchar.h>

namespace logger {
    static std::ofstream log_file;
    static bool log_console = false;
    static bool log_file_enabled = false;
    static std::string log_file_name = "log.txt";

    static std::shared_ptr<spdlog::logger> logger;

    void initialize(bool console, bool file, std::string file_name) {
        std::vector<spdlog::sink_ptr> sinks;

        if (console) {
            setConsole(true);
        }

        sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks[0]->set_pattern("%^[%T] %n: %v%$");

        if (file) {
            sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_name, true));
            sinks[1]->set_pattern("[%T] [%l] %n: %v");
        }

        logger = std::make_shared<spdlog::logger>("OpenHack", begin(sinks), end(sinks));
        spdlog::register_logger(logger);
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);
    }

    void setConsole(bool enabled) {
#ifdef PLATFORM_WINDOWS
        if (enabled) {
            AllocConsole();
            FILE *fDummy;
            freopen_s(&fDummy, "CONIN$", "r", stdin);
            freopen_s(&fDummy, "CONOUT$", "w", stdout);
            freopen_s(&fDummy, "CONOUT$", "w", stderr);
        } else {
            FreeConsole();
        }
#endif
    }

    spdlog::logger *getLogger() {
        return logger.get();
    }
}

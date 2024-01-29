#include "logger.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <windows.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "utils.h"
#include "project.h"
#include <errhandlingapi.h>
#include <DbgHelp.h>
#include <string>
#include <sstream>
#include <MinHook.h>
#pragma comment(lib, "DbgHelp")

namespace crashhandler
{
    std::wstring getModuleName(HMODULE module, bool fullPath = true)
    {
        wchar_t buffer[MAX_PATH];
        if (!GetModuleFileNameW(module, buffer, MAX_PATH))
            return L"Unknown";
        if (fullPath)
            return buffer;
        return std::filesystem::path(buffer).filename().wstring();
    }

    std::wstring getExceptionCodeString(DWORD code)
    {
        switch (code)
        {
        case EXCEPTION_ACCESS_VIOLATION:
            return L"EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            return L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_STACK_OVERFLOW:
            return L"EXCEPTION_STACK_OVERFLOW";
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return L"EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:
            return L"EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_BREAKPOINT:
            return L"EXCEPTION_BREAKPOINT";
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            return L"EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            return L"EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            return L"EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:
            return L"EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:
            return L"EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:
            return L"EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return L"EXCEPTION_INT_DIVIDE_BY_ZERO";
        default:
            return L"Unknown Exception Code";
        }
    }

    void printAddr(std::wstring &result, const void *addr, bool fullPath = true)
    {
        HMODULE module = NULL;

        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)addr, &module))
        {
            const auto diff = reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(module);
            wchar_t buffer[512];
            swprintf_s(buffer, L"%s + %IX", getModuleName(module, fullPath).c_str(), diff);
            result += buffer;
        }
        else
        {
            wchar_t buffer[512];
            swprintf_s(buffer, L"%p", addr);
            result += buffer;
        }
    }

    void printRegisterStates(std::wstring &content, PCONTEXT context)
    {
        wchar_t buffer[200];

        swprintf_s(
            buffer,
            L"- EAX: %08X\n"
            L"- EBX: %08X\n"
            L"- ECX: %08X\n"
            L"- EDX: %08X\n"
            L"- ESI: %08X\n"
            L"- EDI: %08X\n"
            L"- EBP: %08X\n"
            L"- ESP: %08X\n"
            L"- EIP: %08X\n\n",
            context->Eax, context->Ebx, context->Ecx, context->Edx,
            context->Esi, context->Edi, context->Ebp, context->Esp, context->Eip);

        content += buffer;
    }

    void walkStack(std::wstring &result, PCONTEXT context)
    {
        STACKFRAME64 stack;
        memset(&stack, 0, sizeof(STACKFRAME64));

        auto process = GetCurrentProcess();
        auto thread = GetCurrentThread();
        stack.AddrPC.Offset = context->Eip;
        stack.AddrPC.Mode = AddrModeFlat;
        stack.AddrStack.Offset = context->Esp;
        stack.AddrStack.Mode = AddrModeFlat;
        stack.AddrFrame.Offset = context->Ebp;
        stack.AddrFrame.Mode = AddrModeFlat;

        while (true)
        {
            if (!StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, &stack, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
                break;

            wchar_t buffer[512];
            swprintf_s(buffer, L"- ");
            result += buffer;

            printAddr(result, reinterpret_cast<void *>(stack.AddrPC.Offset));
            result += L"\n";
        }
    }

    LONG WINAPI handler(EXCEPTION_POINTERS *info)
    {
        std::wstring message;
        message += L"An exception has occurred.\n";
        message += L"Tip: You can use 'CTRL + C' to copy this message.\n\n";

        wchar_t buffer[512];

        message += L"Information:\n";

        std::wstringstream stream;
        stream << L"- Version: " << PROJECT_VERSION << L"\n";
        stream << L"- Build date: " << __DATE__ " " __TIME__ << "\n";
        stream << L"- Geometry Dash Version: " << utils::get_game_version() << L"\n";
        message += stream.str().c_str();

        message += L"\nException I nformation:\n";

        swprintf_s(buffer, L"- Exception Code: %X (%s)\n", info->ExceptionRecord->ExceptionCode, getExceptionCodeString(info->ExceptionRecord->ExceptionCode).c_str());
        message += buffer;

        swprintf_s(buffer, L"- Exception Flags: %X\n", info->ExceptionRecord->ExceptionFlags);
        message += buffer;

        swprintf_s(buffer, L"- Exception Address: %p (", info->ExceptionRecord->ExceptionAddress);
        message += buffer;
        printAddr(message, info->ExceptionRecord->ExceptionAddress, false);
        message += L")\n";

        swprintf_s(buffer, L"- Number Parameters: %d\n", info->ExceptionRecord->NumberParameters);
        message += buffer;

        message += L"\nStack Trace:\n";
        walkStack(message, info->ContextRecord);

        message += L"\nRegister States:\n";
        printRegisterStates(message, info->ContextRecord);

        MessageBoxW(nullptr, message.c_str(), L"Geometry Dash Crashed", MB_OK | MB_ICONERROR);

        return EXCEPTION_CONTINUE_SEARCH;
    }

    typedef LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI *SetUnhandledExceptionFilterPtr)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
    SetUnhandledExceptionFilterPtr SetUnhandledExceptionFilter_Orig = nullptr;

    LPTOP_LEVEL_EXCEPTION_FILTER WINAPI CustomExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
    {
        return SetUnhandledExceptionFilter_Orig(handler);
    }
}

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
        SetUnhandledExceptionFilter(crashhandler::handler);
        MH_CreateHook(
            SetUnhandledExceptionFilter,
            &crashhandler::CustomExceptionFilter,
            (void **)&crashhandler::SetUnhandledExceptionFilter_Orig);
        MH_EnableHook(SetUnhandledExceptionFilter);
    }
}
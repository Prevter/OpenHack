#include "logger.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <windows.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/xchar.h>

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
    bool symbolsInitialized = false;

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

    HMODULE handleFromAddress(void *address)
    {
        HMODULE hModule = NULL;
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)address, &hModule);
        return hModule;
    }

    void printAddr(std::wstringstream &ss, void *address, bool fullPath = true)
    {
        HMODULE hModule = nullptr;

        if (GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCTSTR)address, &hModule))
        {
            auto const diff = (uintptr_t)address - (uintptr_t)hModule;
            ss << getModuleName(hModule, fullPath) << L" + 0x" << std::hex << diff << std::dec;

            if (symbolsInitialized)
            {
                DWORD64 displacement = 0;
                wchar_t buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
                PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
                pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                pSymbol->MaxNameLen = MAX_SYM_NAME;

                auto process = GetCurrentProcess();

                if (SymFromAddr(process, (DWORD64)address, &displacement, pSymbol))
                {
                    ss << L" (" << pSymbol->Name << L" + 0x" << std::hex << displacement << std::dec;

                    IMAGEHLP_LINE64 line;
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    DWORD displacement;

                    if (SymGetLineFromAddr64(process, (DWORD64)address, &displacement, &line))
                    {
                        PCHAR filePath = line.FileName;
                        std::string filePathStr(filePath);
                        std::string sourceRootDir = SOURCE_ROOT_DIR;
                        std::replace(sourceRootDir.begin(), sourceRootDir.end(), '/', '\\');
                        std::replace(filePathStr.begin(), filePathStr.end(), '/', '\\');
                        std::string::size_type i = filePathStr.find(sourceRootDir);
                        if (i != std::string::npos)
                        {
                            filePathStr.erase(i, sourceRootDir.length());
                        }

                        ss << L" in " << std::wstring(filePathStr.begin(), filePathStr.end()) << L":" << line.LineNumber;
                    }

                    ss << L")";
                }
            }
        }
        else
        {
            ss << address;
        }
    }

    std::wstring getRegisters(PCONTEXT ctx)
    {
        return fmt::format(
            L"EAX: {:08x}\n"
            L"EBX: {:08x}\n"
            L"ECX: {:08x}\n"
            L"EDX: {:08x}\n"
            L"EBP: {:08x}\n"
            L"ESP: {:08x}\n"
            L"EDI: {:08x}\n"
            L"ESI: {:08x}\n"
            L"EIP: {:08x}\n",
            ctx->Eax,
            ctx->Ebx,
            ctx->Ecx,
            ctx->Edx,
            ctx->Ebp,
            ctx->Esp,
            ctx->Edi,
            ctx->Esi,
            ctx->Eip);
    }

    std::wstring getStackTrace(PCONTEXT ctx)
    {
        std::wstringstream ss;

        STACKFRAME64 stackFrame;
        memset(&stackFrame, 0, sizeof(STACKFRAME64));

        DWORD machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = ctx->Eip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = ctx->Ebp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = ctx->Esp;
        stackFrame.AddrStack.Mode = AddrModeFlat;

        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();

        while (StackWalk64(
            machineType,
            process,
            thread,
            &stackFrame,
            ctx,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
        {
            if (stackFrame.AddrPC.Offset == 0)
                break;

            ss << L" - ";
            printAddr(ss, (void *)stackFrame.AddrPC.Offset, false);
            ss << L"\n";
        }

        return ss.str();
    }

    std::wstring getInformation(LPEXCEPTION_POINTERS info)
    {
        std::wstringstream ss;

        ss << L"Exception code: " << std::hex << info->ExceptionRecord->ExceptionCode << std::dec 
        << L" (" << getExceptionCodeString(info->ExceptionRecord->ExceptionCode) << L")\n"
        << L"Exception flags: " << info->ExceptionRecord->ExceptionFlags << L"\n"
        << L"Exception address: " << info->ExceptionRecord->ExceptionAddress << L" (";
        printAddr(ss, info->ExceptionRecord->ExceptionAddress, false);
        ss << L")\n"
        << L"Number of parameters: " << info->ExceptionRecord->NumberParameters << L"\n";

        return ss.str();
    }

    std::wstring generateCrashInfo(LPEXCEPTION_POINTERS info)
    {
        std::wstringstream ss;

        ss << L"== Exception Information ==\n";
        ss << getInformation(info) << L"\n";

        ss << L"== Stack Trace ==\n";
        ss << getStackTrace(info->ContextRecord) << L"\n";

        ss << L"== Registers ==\n";
        ss << getRegisters(info->ContextRecord);

        return ss.str();
    }

    LONG WINAPI exceptionHandler(LPEXCEPTION_POINTERS info)
    {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

        symbolsInitialized = SymInitialize(GetCurrentProcess(), NULL, TRUE);

        std::wstring crashInfo = generateCrashInfo(info);

        const char *ver = utils::get_game_version();
        std::wstring gameVersion = std::wstring(ver, ver + strlen(ver));

        std::string currentDir = utils::get_current_directory();
        std::wstring wCurrentDir = std::wstring(currentDir.begin(), currentDir.end());

        auto text = fmt::format(
            L"An exception had occured!\n"
            L"Tip: You can use 'CTRL + C' to copy this message.\n\n"
            L"== OpenHack information ==\n"
            L"- Version: {}\n"
            L"- Build date: {}\n"
            L"- Commit hash: {}\n"
            L"- Game version: {}\n"
            L"- Game path: {}\n\n{}",
            L"" PROJECT_VERSION,
            L"" __DATE__ " " __TIME__,
            L"" LATEST_COMMIT_HASH,
            gameVersion.c_str(),
            wCurrentDir.c_str(),
            crashInfo.c_str());

        MessageBoxW(NULL, text.c_str(), L"Geometry Dash Crashed", MB_ICONERROR);

        // Save crash info to file
        std::wofstream file("crashlog.txt");
        file << text;
        file.close();

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void init()
    {
        AddVectoredExceptionHandler(
            0,
            [](PEXCEPTION_POINTERS ExceptionInfo) -> LONG
            {
                SetUnhandledExceptionFilter(exceptionHandler);
                return EXCEPTION_CONTINUE_SEARCH;
            });
        SetUnhandledExceptionFilter(exceptionHandler);
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

        set_exception_handler();
    }

    spdlog::logger *get_logger()
    {
        return logger.get();
    }

    void set_exception_handler()
    {
        crashhandler::init();
    }
}
#pragma once

#include "../../shared/openhack.hpp"
#include "logger.hpp"

#ifdef PLATFORM_WINDOWS

#include <spdlog/fmt/xchar.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <Windows.h>
#include <errhandlingapi.h>
#include <DbgHelp.h>
#include <string>

// Fix for "_ThrowInfo" on Clang
#ifdef __clang__
struct _ThrowInfo;
#endif

#include <ehdata.h>
#include <sstream>

#pragma comment(lib, "DbgHelp")

#include "ehdata_structs.hpp"

namespace crashhandler {
    bool symbolsInitialized = false;

    std::wstring getModuleName(HMODULE module, bool fullPath = true) {
        wchar_t buffer[MAX_PATH];
        if (!GetModuleFileNameW(module, buffer, MAX_PATH))
            return L"Unknown";
        if (fullPath)
            return buffer;
        return std::filesystem::path(buffer).filename().wstring();
    }

    std::wstring getExceptionCodeString(DWORD code) {
        switch (code) {
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

    HMODULE handleFromAddress(void *address) {
        HMODULE hModule = NULL;
        GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCTSTR) address, &hModule);
        return hModule;
    }

    void printAddr(std::wstringstream &ss, void *address, bool fullPath = true) {
        HMODULE hModule = nullptr;

        if (GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCTSTR) address, &hModule)) {
            auto const diff = (uintptr_t) address - (uintptr_t) hModule;
            ss << getModuleName(hModule, fullPath) << L" + 0x" << std::hex << diff << std::dec;

            if (symbolsInitialized) {
                DWORD64 displacement = 0;
                wchar_t buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
                PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;
                pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                pSymbol->MaxNameLen = MAX_SYM_NAME;

                auto process = GetCurrentProcess();

                if (SymFromAddr(process, (DWORD64) address, &displacement, pSymbol)) {
                    ss << L" (" << pSymbol->Name << L" + 0x" << std::hex << displacement << std::dec;

                    IMAGEHLP_LINE64 line;
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                    DWORD displacement;

                    if (SymGetLineFromAddr64(process, (DWORD64) address, &displacement, &line)) {
                        PCHAR filePath = line.FileName;
                        std::string filePathStr(filePath);
                        std::string sourceRootDir = OPENHACK_SOURCE_DIR;
                        std::replace(sourceRootDir.begin(), sourceRootDir.end(), '/', '\\');
                        std::replace(filePathStr.begin(), filePathStr.end(), '/', '\\');
                        std::string::size_type i = filePathStr.find(sourceRootDir);
                        if (i != std::string::npos) {
                            filePathStr.erase(i, sourceRootDir.length());
                        }

                        ss << L" in " << std::wstring(filePathStr.begin(), filePathStr.end()) << L":"
                           << line.LineNumber;
                    }

                    ss << L")";
                }
            }
        } else {
            ss << address;
        }
    }

    std::wstring getRegisters(PCONTEXT ctx) {
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

    std::wstring getStackTrace(PCONTEXT ctx) {
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
                NULL)) {
            if (stackFrame.AddrPC.Offset == 0)
                break;

            ss << L" - ";
            printAddr(ss, (void *) stackFrame.AddrPC.Offset, false);
            ss << L"\n";
        }

        return ss.str();
    }

    template<typename T, typename U>
    static std::add_const_t<std::decay_t<T>> rebaseAndCast(intptr_t base, U value) {
        // U value -> const T* (base+value)
        return reinterpret_cast<std::add_const_t<std::decay_t<T>>>(base + (ptrdiff_t) (value));
    }

    std::wstring getInformation(LPEXCEPTION_POINTERS info) {
        std::wstringstream ss;

        if (info->ExceptionRecord->ExceptionCode == EH_EXCEPTION_NUMBER) {
            bool isStdException = false;

            auto *exceptionRecord = info->ExceptionRecord;
            auto exceptionObject = exceptionRecord->ExceptionInformation[1];

            // 0 on 32-bit, dll offset on 64-bit
            intptr_t imageBase =
                    exceptionRecord->NumberParameters >= 4 ? (intptr_t) exceptionRecord->ExceptionInformation[3] : 0;

            auto *throwInfo = (_MSVC_ThrowInfo *) exceptionRecord->ExceptionInformation[2];
            if (!throwInfo || !throwInfo->pCatchableTypeArray) {
                ss << "C++ exception: <no SEH data available about the thrown exception>\n";
            } else {
                auto *catchableTypeArray = rebaseAndCast<_MSVC_CatchableTypeArray *>(imageBase,
                                                                                     throwInfo->pCatchableTypeArray);
                auto ctaSize = catchableTypeArray->nCatchableTypes;
                const char *targetName = nullptr;

                for (int i = 0; i < ctaSize; i++) {
                    auto *catchableType = rebaseAndCast<_MSVC_CatchableType *>(imageBase,
                                                                               catchableTypeArray->arrayOfCatchableTypes[i]);
                    auto *ctDescriptor = rebaseAndCast<_MSVC_TypeDescriptor *>(imageBase, catchableType->pType);
                    const char *classname = ctDescriptor->name;

                    if (i == 0) {
                        targetName = classname;
                    }

                    if (strcmp(classname, ".?AVexception@std@@") == 0) {
                        isStdException = true;
                        break;
                    }
                }

                // demangle the name of the thrown object
                std::wstring demangledName;

                if (!targetName || targetName[0] == '\0' || targetName[1] == '\0') {
                    demangledName = L"<Unknown type>";
                } else {
                    char demangledBuf[256];
                    size_t written = UnDecorateSymbolName(targetName + 1, demangledBuf, 256, UNDNAME_NO_ARGUMENTS);
                    if (written == 0) {
                        demangledName = L"<Unknown type>";
                    } else {
                        demangledName = std::wstring(demangledBuf, demangledBuf + written);
                    }
                }

                if (isStdException) {
                    std::exception *excObject = reinterpret_cast<std::exception *>(exceptionObject);
                    ss << L"C++ Exception: " << demangledName << "(\"" << excObject->what() << "\")"
                       << "\n";
                } else {
                    ss << L"C++ Exception: type '" << demangledName << "'\n";
                }
            }
        } else {
            ss << L"Exception code: " << std::hex << info->ExceptionRecord->ExceptionCode << std::dec
               << L" (" << getExceptionCodeString(info->ExceptionRecord->ExceptionCode) << L")\n"
               << L"Exception flags: " << info->ExceptionRecord->ExceptionFlags << L"\n"
               << L"Exception address: " << info->ExceptionRecord->ExceptionAddress << L" (";
            printAddr(ss, info->ExceptionRecord->ExceptionAddress, false);
            ss << L")\n"
               << L"Number of parameters: " << info->ExceptionRecord->NumberParameters << L"\n";
        }

        return ss.str();
    }

    std::wstring generateCrashInfo(LPEXCEPTION_POINTERS info) {
        std::wstringstream ss;

        ss << L"== Exception Information ==\n";
        ss << getInformation(info) << L"\n";

        ss << L"== Stack Trace ==\n";
        ss << getStackTrace(info->ContextRecord) << L"\n";

        ss << L"== Registers ==\n";
        ss << getRegisters(info->ContextRecord);

        return ss.str();
    }

    LONG WINAPI exceptionHandler(LPEXCEPTION_POINTERS info) {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

        symbolsInitialized = SymInitialize(GetCurrentProcess(), NULL, TRUE);

        std::wstring crashInfo = generateCrashInfo(info);

        std::string ver = openhack::utils::getGameVersion();
        std::wstring gameVersion = std::wstring(ver.begin(), ver.end());

        std::string currentDir = openhack::utils::getCurrentDirectory();
        std::wstring wCurrentDir = std::wstring(currentDir.begin(), currentDir.end());

        std::wstring text = fmt::format(
                L"An exception had occured!\n"
                L"Tip: You can use 'CTRL + C' to copy this message.\n\n"
                L"== OpenHack information ==\n"
                L"- Version: {}\n"
                L"- Build date: {}\n"
                L"- Commit hash: {}\n"
                L"- Game version: {}\n"
                L"- Game path: {}\n\n{}",
                L"" OPENHACK_VERSION,
                L"" __DATE__ " " __TIME__,
                L"" OPENHACK_LATEST_COMMIT,
                gameVersion.c_str(),
                wCurrentDir.c_str(),
                crashInfo.c_str());

        MessageBoxW(NULL, text.c_str(), L"Geometry Dash Crashed", MB_ICONERROR);

        // Save crash info to file
        std::wofstream file("latest-crash.log");
        file << text;
        file.close();

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void init() {
        AddVectoredExceptionHandler(
                0,
                [](PEXCEPTION_POINTERS ExceptionInfo) -> LONG {
                    SetUnhandledExceptionFilter(exceptionHandler);
                    return EXCEPTION_CONTINUE_SEARCH;
                });
        SetUnhandledExceptionFilter(exceptionHandler);
    }
}

#else

namespace crashhandler
{
    void init()
    {
    }
}

#endif
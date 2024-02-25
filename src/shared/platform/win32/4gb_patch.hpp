#pragma once

#include <cstdint>
#include <fstream>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace openhack::win32::four_gb {
    /// @brief Whether `isPatched` has been called.
    inline static bool HAS_CHECKED = false;
    /// @brief Whether the game is already patched to use 4GB of memory.
    inline static bool IS_PATCHED = false;

    /// @brief Gets the path of the running executable.
    /// @return The path of the running executable.
    inline std::string getExecutablePath() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        return buffer;
    }

    /// @brief Check whether the game is already patched to use 4GB of memory.
    /// @return Whether the game is already patched to use 4GB of memory.
    inline bool isPatched() {
        // Check if we already know the result
        if (HAS_CHECKED) return IS_PATCHED;

        auto exePath = getExecutablePath();

        // Try reading the game executable
        std::ifstream file(exePath, std::ios::binary);
        if (!file.is_open()) return false;

        // Read the file into memory
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        auto buffer = new char[size];
        file.read(buffer, size);
        file.close();

        // Check if the "largeAddressAware" flag is set
        auto peHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(buffer +
                                                            reinterpret_cast<PIMAGE_DOS_HEADER>(buffer)->e_lfanew);
        IS_PATCHED = (peHeader->FileHeader.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE) != 0;

        // Clean up
        delete[] buffer;

        // Return the result of the check
        HAS_CHECKED = true;
        return IS_PATCHED;
    }

    /// @brief Patch the game to use 4GB of memory.
    /// @return Whether the patch was successful.
    inline bool patch() {
        // Try reading the game executable
        auto exePath = getExecutablePath();
        std::ifstream file(exePath, std::ios::binary);
        if (!file.is_open()) return false;

        // Read the file into memory
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        auto buffer = new char[size];
        file.read(buffer, size);
        file.close();

        // Set the "largeAddressAware" flag
        auto peHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(buffer +
                                                            reinterpret_cast<PIMAGE_DOS_HEADER>(buffer)->e_lfanew);
        peHeader->FileHeader.Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE;

        // Move the original file to a backup
        auto backupPath = exePath + ".bak";
        if (std::rename(exePath.c_str(), backupPath.c_str()) != 0) {
            delete[] buffer;
            return false;
        }

        // Write the modified file
        std::ofstream newFile(exePath, std::ios::binary);
        newFile.write(buffer, size);
        newFile.close();

        // Clean up
        delete[] buffer;
        return true;
    }
}
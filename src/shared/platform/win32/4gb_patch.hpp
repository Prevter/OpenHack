#pragma once

#include <cstdint>
#include <fstream>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace openhack::win32::four_gb {
    /// @brief Whether `isPatched` has been called.
    inline static bool HAS_CHECKED = false;
    /// @brief Whether the game is already patched to use 4GB of memory.
    inline static bool IS_PATCHED = false;

    /// @brief Check whether the game is already patched to use 4GB of memory.
    /// @return Whether the game is already patched to use 4GB of memory.
    inline bool isPatched() {
        // Check if we already know the result
        if (HAS_CHECKED) return IS_PATCHED;

        // Read 'GeometryDash.exe' into memory
        std::ifstream file("GeometryDash.exe", std::ios::binary);
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
        // Read 'GeometryDash.exe' into memory
        std::ifstream file("GeometryDash.exe", std::ios::binary);
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
        auto backupPath = "GeometryDash.exe.bak";
        if (std::rename("GeometryDash.exe", backupPath) != 0) {
            delete[] buffer;
            return false;
        }

        // Write the modified file
        std::ofstream newFile("GeometryDash.exe", std::ios::binary);
        newFile.write(buffer, size);
        newFile.close();

        // Clean up
        delete[] buffer;
        return true;
    }
}
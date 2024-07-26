#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Psapi.h>

/// @brief Contains tools for signature scanning.
namespace gd::sigscan {
    /*
     * Pattern syntax:
     * ?  - any byte
     * ^  - set address cursor
     * *  - multi pattern (finds all matches)
     * 1F - byte value (any hex value)
     *
     * Example:
     * "*6A106A0CE8????84C0^750B8BCE"
     * 1. '*' tells to find all occurrences of the pattern
     * 2. Then match this pattern:
     * 6A 10 6A 0C E8 XX XX XX XX 84 C0 75 0B 8B CE
     *                                 ^
     * 3. Store address of the byte marked with '^' in the result
     *
     * This will return two addresses which we can use to patch the game.
     *
    */

    /*
     * Mask syntax:
     * ?                 - any byte
     * %([+/-]XX)        - add or subtract XX from original byte value
     * $([+/-]XX)        - get byte value from original_address + XX as offset
     * &([+/-]XX[+/-]YY) - combines % and $ (XX is offset, YY is value to add/subtract)
     * *XX               - repeat next mask token XX times
     * @N(PATTERN)       - look for a pattern and store a relative offset in N bytes (used for relative jumps)
     * 1F                - byte value (any hex value)
     *
     * Example 1:
     * "909090909090C6?%(-3A)2D??01E9"
     * 1. First 7 bytes ("909090909090C6") are simply replaced
     * 2. Next byte ("?") is left as is
     * 3. "%(-3A)" tells to subtract 0x3A from original byte value
     * 4. "2D ? ? 01 E9" does the same as step 1-2
     *
     * Example 2:
     * "E9&(01+01)*03$0190"
     * 1. "E9" is replaced
     * 2. "&(01+01)" takes the value of the byte with offset 0x01 (next byte),
     * adds 0x01 to it and stores it here
     * 3. "*03" tells to repeat the next mask token 3 times:
     *     - "$01" gets the value of the next byte
     * 4. "90" is replaced
     *
     * Example 3:
     * "E9@4(8BCBFF15????^8B4C24?64890D00000000595F5E)"
     * 1. "E9" is replaced (jmp)
     * 2. "@4" tells to look for a pattern and store a relative offset in 4 bytes
     * 3. "(8BCBFF15????^8B4C24?64890D00000000595F5E)" is the pattern to look for (see pattern syntax)
     * If the offset for this pattern is 0x746, the final mask would be:
     * "E942070000" (jmp 0x747)
     * The changes from 0x746 to 0x742 and to 0x747 are explained by how relative jumps work in x86:
     * - 0x746 to 0x742 is calculated by subtracting 4 bytes because the jump address is 4 bytes long
     * - "E9 42070000" is actually "jmp 0x747" because the jump takes 5 bytes and jumps from it's end (0x742 + 5 = 0x747)
     *
    */

    /// @brief A pattern token
    struct PatternToken {
        bool any_byte;           // true if the byte is a wildcard
        uint8_t byte;            // value of the byte
        bool set_address_cursor; // true if the address cursor should be set to this byte
        bool multi_pattern;      // true if the pattern should be matched multiple times
        int8_t jump_if_fail;     // valid inside [] brackets, tells how many tokens to skip in the pattern if the match fails

        PatternToken() : any_byte(false), byte(0), set_address_cursor(false), multi_pattern(false), jump_if_fail(0) {}
    };

    /// @brief A mask token
    struct MaskToken {
        bool any_byte; // true if the byte is a wildcard

        bool is_pattern;  // true if the value should be an address to the pattern
        bool is_relative; // true if the byte value should be changed by a relative offset
        bool is_address;  // true if the value of the byte should be taken from original_address + offset

        // NOTE: if is_relative and is_address are both true,
        // the value of the byte will be taken from original_address + offset + value

        uint8_t value; // value to change the byte to
        int8_t offset; // offset to add to the value

        std::string pattern; // pattern to calculate the offset from

        MaskToken() : any_byte(false), is_pattern(false), is_relative(false), is_address(false), value(0), offset(0) {}
    };

    /// @brief Result of a pattern match
    struct Opcode {
        uintptr_t address;
        std::string library;
        std::vector<uint8_t> original;
        std::vector<uint8_t> patched;

        Opcode(uintptr_t address, std::string library, std::vector<uint8_t> original,
               std::vector<uint8_t> patched)
                : address(address),
                  library(std::move(library)),
                  original(std::move(original)),
                  patched(std::move(patched)) {}
    };

    inline PatternToken parseToken(std::string pattern, uint32_t &currentIndex) {
        PatternToken token;
        token.any_byte = false;
        token.byte = 0;
        token.set_address_cursor = false;
        token.multi_pattern = false;
        token.jump_if_fail = -1;

        if (pattern[currentIndex] == '?') {
            token.any_byte = true;
            currentIndex++;
        } else if (pattern[currentIndex] == '^') {
            token.set_address_cursor = true;
            currentIndex++;
        } else if (pattern[currentIndex] == '*') {
            token.multi_pattern = true;
            currentIndex++;
        } else {
            token.byte = std::stoi(pattern.substr(currentIndex, 2), nullptr, 16);
            currentIndex += 2;
        }

        return token;
    }

    inline bool eatToken(const char symbol, const std::string &pattern, uint32_t &currentIndex) {
        if (pattern[currentIndex] == symbol) {
            currentIndex++;
            return true;
        }
        return false;
    }

    inline int8_t readSignedByte(const std::string &pattern, uint32_t &currentIndex) {
        bool is_negative = false;
        switch (pattern[currentIndex]) {
            case '+':
                currentIndex++;
                break;
            case '-':
                is_negative = true;
                currentIndex++;
                break;
            default:
                break;
        }

        int8_t byte = std::stoi(pattern.substr(currentIndex, 2), nullptr, 16);
        if (is_negative)
            byte = -byte;
        currentIndex += 2;

        return byte;
    }

    /// @brief Parses a pattern string to get a list of tokens
    /// @param pattern Pattern string
    /// @return List of pattern tokens
    inline std::vector<PatternToken> parsePattern(const std::string &pattern) {
        uint32_t current_index = 0;
        std::vector<PatternToken> tokens;

        while (current_index < pattern.length()) {
            if (pattern[current_index] == ' ') {
                current_index++;
                continue;
            } else if (eatToken('[', pattern, current_index)) {
                std::vector<PatternToken> sub_tokens;
                while (!eatToken(']', pattern, current_index)) {
                    PatternToken token = parseToken(pattern, current_index);
                    sub_tokens.push_back(token);
                }

                int32_t len = sub_tokens.size();
                for (uint32_t i = 0; i < sub_tokens.size(); i++) {
                    sub_tokens[i].jump_if_fail = len - i - 1;
                }

                tokens.insert(tokens.end(), sub_tokens.begin(), sub_tokens.end());
                continue;
            }

            PatternToken token = parseToken(pattern, current_index);
            tokens.push_back(token);
        }

        return tokens;
    }

    /// @brief Parses a mask string to get a list of tokens
    /// @param mask Mask string
    /// @return List of mask tokens
    inline std::vector<MaskToken> parseMask(const std::string &mask) {
        uint32_t current_index = 0;
        std::vector<MaskToken> bytes;

        uint32_t repeat_count = 1;

        while (current_index < mask.length()) {
            MaskToken byte;
            byte.any_byte = false;
            byte.is_relative = false;
            byte.is_address = false;
            byte.value = 0;
            byte.offset = 0;

            if (mask[current_index] == ' ') // ignore spaces
            {
                current_index++;
            } else if (mask[current_index] == '?') // wildcard byte
            {
                byte.any_byte = true;
                current_index++;
            } else if (mask[current_index] == '%') // add/sub byte
            {
                byte.is_relative = true;
                eatToken('(', mask, ++current_index);
                byte.value = readSignedByte(mask, current_index);
                eatToken(')', mask, current_index);
            } else if (mask[current_index] == '$') // address byte
            {
                byte.is_address = true;
                eatToken('(', mask, ++current_index);
                byte.offset = readSignedByte(mask, current_index);
                eatToken(')', mask, current_index);
            } else if (mask[current_index] == '&') // address + add/sub byte
            {
                byte.is_address = true;
                byte.is_relative = true;
                eatToken('(', mask, ++current_index);
                byte.offset = readSignedByte(mask, current_index);
                byte.value = readSignedByte(mask, current_index);
                eatToken(')', mask, current_index);
            } else if (mask[current_index] == '*') // repeat next byte n times
            {
                eatToken('(', mask, ++current_index);
                repeat_count = std::stoi(mask.substr(current_index, 2), nullptr, 16);
                current_index += 2;
                eatToken(')', mask, current_index);
                continue;
            } else if (mask[current_index] == '@') // calculate offset by pattern
            {
                byte.is_pattern = true;
                // read how many bytes should be stored
                uint8_t bytes_to_store = std::stoi(mask.substr(++current_index, 1), nullptr, 10);
                eatToken('(', mask, ++current_index);

                // read the pattern
                std::string pattern;
                while (!eatToken(')', mask, current_index)) {
                    pattern += mask[current_index];
                    current_index++;
                }

                byte.value = bytes_to_store;
                byte.pattern = pattern;
            } else {
                byte.value = std::stoi(mask.substr(current_index, 2), nullptr, 16);
                current_index += 2;
            }

            for (uint32_t i = 0; i < repeat_count; i++)
                bytes.push_back(byte);

            repeat_count = 1;
        }

        return bytes;
    }

    /// @brief Finds a pattern in a library
    /// @param pattern A list of pattern tokens
    /// @param library Library name
    /// @return All found addresses
    inline std::vector<uintptr_t> findPattern(std::vector<PatternToken> pattern, const std::string &library = "") {
        // get module handle
        HMODULE module;
        if (library.empty())
            module = GetModuleHandle(nullptr);
        else
            module = GetModuleHandle(library.c_str());

        if (module == nullptr)
            return {};

        bool search_all = false;

        // check if first token is a wildcard
        if (pattern[0].multi_pattern) {
            search_all = true;
            pattern.erase(pattern.begin());
        }

        // get module info
        MODULEINFO module_info;
        if (!GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(MODULEINFO)))
            return {};

        std::vector<uintptr_t> addresses;

        // iterate over module memory
        for (uint32_t i = 0; i < module_info.SizeOfImage; i++) {
            bool found = true;
            void *address = (void *) ((uintptr_t) module + i);
            void *addr = address;
            bool set_address_cursor = false;
            uint32_t subtracted_bytes = 0;
            uint32_t max_jump_length = -1;

            // iterate over pattern tokens
            for (uint32_t j = 0; j < pattern.size(); j++) {
                // check if we need to set the address cursor
                if (pattern[j].set_address_cursor) {
                    // set address to current address
                    addr = (void *) ((uintptr_t) address + j);
                    set_address_cursor = true;
                    continue;
                }

                // if cursor has been set, we need to offset j by 1
                uintptr_t addr = (uintptr_t) address + (set_address_cursor ? j - 1 : j) - subtracted_bytes;

                // check if we have enough memory left
                if (addr >= (uintptr_t) module + module_info.SizeOfImage) {
                    found = false;
                    break;
                }

                if (pattern[j].jump_if_fail != -1 && max_jump_length == -1) {
                    max_jump_length = pattern[j].jump_if_fail;
                } else if (pattern[j].jump_if_fail == -1) {
                    max_jump_length = -1;
                }

                // check if we have a match
                if (!pattern[j].any_byte && *(uint8_t *) (addr) != pattern[j].byte) {
                    // check if has "jump_if_fail" set
                    if (pattern[j].jump_if_fail != -1) {
                        // jump to the next token
                        j += pattern[j].jump_if_fail;

                        // we also need to make sure we return to the original address before the [] brackets
                        subtracted_bytes += max_jump_length + 1;
                        continue;
                    }

                    found = false;
                    break;
                }
            }

            // return address offset for base address
            if (found) {
                if (search_all)
                    addresses.push_back((uintptr_t) addr);
                else
                    return {(uintptr_t) addr};
            }
        }

        return addresses;
    }

    /// @brief Finds a pattern in a library
    /// @param pattern Pattern string
    /// @param library Library name
    /// @return All found addresses
    inline std::vector<uintptr_t> findPatterns(const std::string &pattern, const std::string &library = "") {
        return findPattern(parsePattern(pattern), library);
    }

    /// @brief Finds a pattern in a library
    /// @param pattern Pattern string
    /// @param library Library name
    /// @return The first found address
    inline uintptr_t findPattern(const std::string &pattern, const std::string &library = "") {
        auto result = findPatterns(pattern, library);
        if (result.empty()) {
            return 0;
        }
        return result[0];
    }

    /// @brief Finds a pattern in a library
    /// @param pattern Pattern string
    /// @param mask Mask string
    /// @param library Library name
    /// @return All opcodes found
    inline std::vector<Opcode>
    match(const std::string &pattern, const std::string &mask = "", const std::string &library = "") {
        std::vector<Opcode> results;

        auto patternTokens = parsePattern(pattern);
        auto maskTokens = parseMask(mask);

        // find pattern
        auto addresses = findPattern(patternTokens, library);
        if (addresses.empty())
            return results;

        for (auto &address: addresses) {
            // address is the base address, so we need to offset it
            uintptr_t module_addr;
            if (library.empty())
                module_addr = (uintptr_t) GetModuleHandle(nullptr);
            else
                module_addr = (uintptr_t) GetModuleHandle(library.c_str());

            Opcode opcode((uintptr_t) address - module_addr, library, {}, {});
            uintptr_t global_offset = 0;


            // read bytes
            for (uint32_t i = 0; i < maskTokens.size(); i++) {
                uintptr_t curr_address = (uintptr_t) address + i + global_offset;
                uint8_t byte = *(uint8_t *) curr_address;
                opcode.original.push_back(byte);

                if (maskTokens[i].any_byte) {
                    // add wildcard byte
                    opcode.patched.push_back(byte);
                } else if (maskTokens[i].is_relative && maskTokens[i].is_address) {
                    // take byte from relative address and add value
                    uint8_t value = *(uint8_t *) (curr_address + maskTokens[i].offset);
                    opcode.patched.push_back(value + maskTokens[i].value);
                } else if (maskTokens[i].is_relative) {
                    // add value to current byte
                    opcode.patched.push_back(byte + maskTokens[i].value);
                } else if (maskTokens[i].is_address) {
                    // take byte from relative address
                    uint8_t value = *(uint8_t *) (curr_address + maskTokens[i].offset);
                    opcode.patched.push_back(value);
                } else if (maskTokens[i].is_pattern) {
                    // find pattern and calculate offset
                    uintptr_t addr = findPattern(maskTokens[i].pattern, library);

                    uintptr_t offset = (uintptr_t) addr - (uintptr_t) curr_address;
                    offset -= maskTokens[i].value;

                    // format offset to bytes
                    for (uint32_t j = 0; j < maskTokens[i].value; j++) {
                        curr_address = (uintptr_t) address + i + global_offset + j;
                        uint8_t byte = *(uint8_t *) curr_address;

                        if (j > 0) // first byte was added already
                            opcode.original.push_back(byte);

                        opcode.patched.push_back((offset >> (j * 8)) & 0xFF);
                    }

                    global_offset += maskTokens[i].value;
                } else {
                    // set byte to a specific value
                    opcode.patched.push_back(maskTokens[i].value);
                }
            }

            results.push_back(opcode);
        }

        return results;
    }
}
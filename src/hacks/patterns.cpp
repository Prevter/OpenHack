#include "patterns.h"
#include "Psapi.h"

namespace patterns
{
    std::vector<token_t> parse_pattern(std::string pattern)
    {
        uint32_t current_index = 0;
        std::vector<token_t> tokens;

        while (current_index < pattern.length())
        {
            token_t token;
            token.any_byte = false;
            token.byte = 0;
            token.set_address_cursor = false;

            if (pattern[current_index] == ' ')
            {
                current_index++;
            }
            else if (pattern[current_index] == '?')
            {
                token.any_byte = true;
                current_index++;
            }
            else if (pattern[current_index] == '^')
            {
                token.set_address_cursor = true;
                current_index++;
            }
            else
            {
                token.byte = std::stoi(pattern.substr(current_index, 2), nullptr, 16);
                current_index += 2;
            }

            tokens.push_back(token);
        }

        return tokens;
    }

    bool eat_token(char symbol, std::string &pattern, uint32_t &current_index)
    {
        if (pattern[current_index] == symbol)
        {
            current_index++;
            return true;
        }

        return false;
    }

    std::vector<byte_t> parse_mask(std::string mask)
    {
        uint32_t current_index = 0;
        std::vector<byte_t> bytes;

        while (current_index < mask.length())
        {
            byte_t byte;
            byte.any_byte = false;
            byte.is_relative = false;
            byte.byte = 0;

            if (mask[current_index] == ' ')
            {
                current_index++;
            }
            else if (mask[current_index] == '?')
            {
                byte.any_byte = true;
                current_index++;
            }
            else if (mask[current_index] == '%')
            {
                byte.is_relative = true;
                eat_token('(', mask, ++current_index);
                bool is_negative = false;
                switch (mask[current_index])
                {
                case '+':
                    current_index++;
                    break;
                case '-':
                    is_negative = true;
                    current_index++;
                    break;
                default:
                    break;
                }
                byte.byte = std::stoi(mask.substr(current_index, 2), nullptr, 16);
                if (is_negative)
                    byte.byte = -byte.byte;
                current_index += 2;
                eat_token(')', mask, current_index);
            }
            else
            {
                byte.byte = std::stoi(mask.substr(current_index, 2), nullptr, 16);
                current_index += 2;
            }

            bytes.push_back(byte);
        }

        return bytes;
    }

    void *find_pattern(std::vector<token_t> pattern, std::string library)
    {
        // get module handle
        HMODULE module;
        if (library == "")
            module = GetModuleHandle(0);
        else
            module = GetModuleHandle(library.c_str());

        if (module == nullptr)
            return nullptr;

        // get module info
        MODULEINFO module_info;
        if (!GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(MODULEINFO)))
            return nullptr;

        // iterate over module memory
        for (uint32_t i = 0; i < module_info.SizeOfImage; i++)
        {
            bool found = true;
            void *address = (void *)((uintptr_t)module + i);
            bool set_address_cursor = false;

            // iterate over pattern tokens
            for (uint32_t j = 0; j < pattern.size(); j++)
            {
                // check if we need to set the address cursor
                if (pattern[j].set_address_cursor)
                {
                    address = (void *)((uintptr_t)address + j);
                    set_address_cursor = true;
                    continue;
                }

                // if cursor has been set, we need to offset j by 1
                uintptr_t addr = (uintptr_t)address + (set_address_cursor ? j - 1 : j);

                // check if we have enough memory left
                if (addr >= (uintptr_t)module + module_info.SizeOfImage)
                {
                    found = false;
                    break;
                }

                // check if we have a match
                if (!pattern[j].any_byte && *(uint8_t *)(addr) != pattern[j].byte)
                {
                    found = false;
                    break;
                }
            }

            // return address offset for base address
            if (found)
            {
                return address;
            }
        }

        return nullptr;
    }

    result_t match(std::string pattern, std::string library, std::string mask)
    {
        result_t result;
        result.found = false;
        result.address = nullptr;

        // parse pattern and mask
        std::vector<token_t> tokens = parse_pattern(pattern);
        std::vector<byte_t> bytes = parse_mask(mask);

        // find pattern
        void *address = find_pattern(tokens, library);
        if (address == nullptr)
            return result;

        // set result
        result.found = true;

        // address is the base address, so we need to offset it
        uintptr_t module_addr;
        if (library == "")
            module_addr = (uintptr_t)GetModuleHandle(0);
        else
            module_addr = (uintptr_t)GetModuleHandle(library.c_str());
        result.address = (void *)((uintptr_t)address - module_addr);

        // read bytes
        for (uint32_t i = 0; i < bytes.size(); i++)
        {
            uint8_t byte = *(uint8_t *)((uintptr_t)address + i);
            result.off_bytes.push_back(byte);

            if (bytes[i].any_byte)
            {
                // add wildcard byte
                result.on_bytes.push_back(byte);
            }
            else if (bytes[i].is_relative)
            {
                // add some value to the byte
                result.on_bytes.push_back(byte + bytes[i].byte);
            }
            else
            {
                // set byte to a specific value
                result.on_bytes.push_back(bytes[i].byte);
            }
        }

        return result;
    }
}
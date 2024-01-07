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
            else if (pattern[current_index] == '*')
            {
                token.multi_pattern = true;
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

    int8_t read_signed_byte(std::string &pattern, uint32_t &current_index)
    {
        bool is_negative = false;
        switch (pattern[current_index])
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

        int8_t byte = std::stoi(pattern.substr(current_index, 2), nullptr, 16);
        if (is_negative)
            byte = -byte;
        current_index += 2;

        return byte;
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
            byte.is_address = false;
            byte.value = 0;
            byte.offset = 0;

            if (mask[current_index] == ' ') // ignore spaces
            {
                current_index++;
            }
            else if (mask[current_index] == '?') // wildcard byte
            {
                byte.any_byte = true;
                current_index++;
            }
            else if (mask[current_index] == '%') // add/sub byte
            {
                byte.is_relative = true;
                eat_token('(', mask, ++current_index);
                byte.value = read_signed_byte(mask, current_index);
                eat_token(')', mask, current_index);
            }
            else if (mask[current_index] == '$') // address byte
            {
                byte.is_address = true;
                eat_token('(', mask, ++current_index);
                byte.offset = read_signed_byte(mask, current_index);
                eat_token(')', mask, current_index);
            }
            else if (mask[current_index] == '&') // address + add/sub byte
            {
                byte.is_address = true;
                byte.is_relative = true;
                eat_token('(', mask, ++current_index);
                byte.offset = read_signed_byte(mask, current_index);
                byte.value = read_signed_byte(mask, current_index);
                eat_token(')', mask, current_index);
            }
            else
            {
                byte.value = std::stoi(mask.substr(current_index, 2), nullptr, 16);
                current_index += 2;
            }

            bytes.push_back(byte);
        }

        return bytes;
    }

    std::vector<uintptr_t> find_pattern(std::vector<token_t> pattern, std::string library)
    {
        // get module handle
        HMODULE module;
        if (library == "")
            module = GetModuleHandle(0);
        else
            module = GetModuleHandle(library.c_str());

        if (module == nullptr)
            return {};

        bool search_all = false;

        // check if first token is a wildcard
        if (pattern[0].multi_pattern)
        {
            search_all = true;
            pattern.erase(pattern.begin());
        }

        // get module info
        MODULEINFO module_info;
        if (!GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(MODULEINFO)))
            return {};

        std::vector<uintptr_t> addresses;

        // iterate over module memory
        for (uint32_t i = 0; i < module_info.SizeOfImage; i++)
        {
            bool found = true;
            void *address = (void *)((uintptr_t)module + i);
            void *addr = address;
            bool set_address_cursor = false;

            // iterate over pattern tokens
            for (uint32_t j = 0; j < pattern.size(); j++)
            {
                // check if we need to set the address cursor
                if (pattern[j].set_address_cursor)
                {
                    // set address to current address
                    addr = (void *)((uintptr_t)address + j);
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
                if (search_all)
                    addresses.push_back((uintptr_t)addr);
                else
                    return {(uintptr_t)addr};
            }
        }

        return addresses;
    }

    result_t match(std::string pattern, std::string library, std::string mask)
    {
        result_t result;
        result.found = false;

        // parse pattern and mask
        std::vector<token_t> tokens = parse_pattern(pattern);
        std::vector<byte_t> bytes = parse_mask(mask);

        // find pattern
        auto addresses = find_pattern(tokens, library);
        if (addresses.size() == 0)
            return result;

        // set result
        result.found = true;

        for (auto &address : addresses)
        {
            // address is the base address, so we need to offset it
            uintptr_t module_addr;
            if (library == "")
                module_addr = (uintptr_t)GetModuleHandle(0);
            else
                module_addr = (uintptr_t)GetModuleHandle(library.c_str());

            opcode_t opcode;
            opcode.address = (void *)((uintptr_t)address - module_addr);

            L_TRACE("pattern: {}", pattern);
            L_TRACE("address: 0x{:X}", (uintptr_t)address);
            L_TRACE("relative address: 0x{:X}", (uintptr_t)opcode.address);

            // read bytes
            for (uint32_t i = 0; i < bytes.size(); i++)
            {
                uintptr_t curr_address = (uintptr_t)address + i;
                uint8_t byte = *(uint8_t *)curr_address;
                opcode.off_bytes.push_back(byte);

                L_TRACE(
                    "token(i:{}, wild:{}, rel:{}, addr:{}, val:{:02X}, off:{:02X})",
                    i, bytes[i].any_byte, bytes[i].is_relative,
                    bytes[i].is_address, bytes[i].value, bytes[i].offset);

                if (bytes[i].any_byte)
                {
                    // add wildcard byte
                    opcode.on_bytes.push_back(byte);
                }
                else if (bytes[i].is_relative && bytes[i].is_address)
                {
                    // take byte from relative address and add value
                    uint8_t value = *(uint8_t *)(curr_address + bytes[i].offset);
                    opcode.on_bytes.push_back(value + bytes[i].value);
                }
                else if (bytes[i].is_relative)
                {
                    // add value to current byte
                    opcode.on_bytes.push_back(byte + bytes[i].value);
                }
                else if (bytes[i].is_address)
                {
                    // take byte from relative address
                    uint8_t value = *(uint8_t *)(curr_address + bytes[i].offset);
                    opcode.on_bytes.push_back(value);
                }
                else
                {
                    // set byte to a specific value
                    opcode.on_bytes.push_back(bytes[i].value);
                }

                L_TRACE("off: 0x{:02X}, on: 0x{:02X}", opcode.off_bytes[i], opcode.on_bytes[i]);
            }

            result.opcodes.push_back(opcode);
        }

        return result;
    }
}
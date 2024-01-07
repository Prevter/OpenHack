#pragma once
#include "../pch.h"

namespace patterns
{
    // Either a byte or a wildcard
    struct byte_t
    {
        bool any_byte; // true if the byte is a wildcard
        bool is_relative; // true if the byte value should be changed by a relative offset
        uint8_t byte; // value to change the byte to
    };

    // The result of a pattern match
    struct result_t
    {
        bool found;
        void *address;
        std::vector<uint8_t> on_bytes;
        std::vector<uint8_t> off_bytes;
    };

    // A token in a pattern string
    struct token_t
    {
        bool any_byte;
        uint8_t byte;
        bool set_address_cursor;
    };

    // Parses a pattern string and returns a vector of tokens
    std::vector<token_t> parse_pattern(std::string pattern);

    // Parse a mask string and returns a vector of bytes
    std::vector<byte_t> parse_mask(std::string mask);

    // Finds a pattern in a library and returns the address
    void *find_pattern(std::vector<token_t> pattern, std::string library);

    // Parses a pattern string and returns a result_t
    result_t match(std::string pattern, std::string library = "", std::string mask = "");
}
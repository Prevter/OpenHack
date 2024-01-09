#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace patterns
{
    /*

    Pattern syntax:
    ?  - any byte
    ^  - set address cursor
    *  - multi pattern (finds all matches)
    1F - byte value (any hex value)

    Example:
    "*6A106A0CE8????84C0^750B8BCE"
    1. '*' tells to find all occurences of the pattern
    2. Then match this pattern:
    6A 10 6A 0C E8 XX XX XX XX 84 C0 75 0B 8B CE
                                    ^
    3. Store address of the byte marked with '^' in the result

    This will return two addresses which we can use to patch the game.

    */

    /*

    Mask syntax:
    ?                 - any byte
    %([+/-]XX)        - add or subtract XX from original byte value
    $([+/-]XX)        - get byte value from original_address + XX as offset
    &([+/-]XX[+/-]YY) - combines % and $ (XX is offset, YY is value to add/subtract)
    *XX               - repeat next mask token XX times
    1F                - byte value (any hex value)

    Example 1:
    "909090909090C6?%(-3A)2D??01E9"
    1. First 7 bytes ("909090909090C6") are simply replaced
    2. Next byte ("?") is left as is
    3. "%(-3A)" tells to subtract 0x3A from original byte value
    4. "2D ? ? 01 E9" does the same as step 1-2

    Example 2:
    "E9&(01+01)*03$0190"
    1. "E9" is replaced
    2. "&(01+01)" takes the value of the byte with offset 0x01 (next byte),
    adds 0x01 to it and stores it here
    3. "*03" tells to repeat the next mask token 3 times:
        - "$01" gets the value of the next byte
    4. "90" is replaced

    */

    // Either a byte or a wildcard
    struct byte_t
    {
        bool any_byte;    // true if the byte is a wildcard
        bool is_relative; // true if the byte value should be changed by a relative offset
        bool is_address;  // true if the value of the byte should be taken from original_address + offset

        // NOTE: if is_relative and is_address are both true,
        // the value of the byte will be taken from original_address + offset + value

        uint8_t value; // value to change the byte to
        int8_t offset; // offset to add to the value
    };

    struct opcode_t
    {
        void *address;
        std::vector<uint8_t> on_bytes;
        std::vector<uint8_t> off_bytes;
    };

    // The result of a pattern match
    struct result_t
    {
        bool found;
        std::vector<opcode_t> opcodes;
    };

    // A token in a pattern string
    struct token_t
    {
        bool any_byte;
        uint8_t byte;
        bool set_address_cursor;
        bool multi_pattern;
    };

    // Parses a pattern string and returns a vector of tokens
    std::vector<token_t> parse_pattern(std::string pattern);

    // Parse a mask string and returns a vector of bytes
    std::vector<byte_t> parse_mask(std::string mask);

    // Finds a pattern in a library and returns the address (or addresses if pattern contains *)
    std::vector<uintptr_t> find_pattern(std::vector<token_t> pattern, std::string library = "");

    // methods for finding only addresses
    std::vector<uintptr_t> find_patterns(std::string pattern, std::string library = "");
    uintptr_t find_pattern(std::string pattern, std::string library = "");

    // Parses a pattern string and returns a result_t
    result_t match(std::string pattern, std::string library = "", std::string mask = "");
}
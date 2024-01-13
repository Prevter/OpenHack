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
    @N(PATTERN)       - look for a pattern and store a relative offset in N bytes (used for relative jumps)
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

    Example 3:
    "E9@4(8BCBFF15????^8B4C24?64890D00000000595F5E)"
    1. "E9" is replaced (jmp)
    2. "@4" tells to look for a pattern and store a relative offset in 4 bytes
    3. "(8BCBFF15????^8B4C24?64890D00000000595F5E)" is the pattern to look for (see pattern syntax)
    If the offset for this pattern is 0x746, the final mask would be:
    "E942070000" (jmp 0x747)
    The changes from 0x746 to 0x742 and to 0x747 are explained by how relative jumps work in x86:
    - 0x746 to 0x742 is calculated by subtracting 4 bytes because the jump address is 4 bytes long
    - "E9 42070000" is actually "jmp 0x747" because the jump takes 5 bytes and jumps from it's end (0x742 + 5 = 0x747)

    */

    // Either a byte or a wildcard
    struct byte_t
    {
        bool any_byte; // true if the byte is a wildcard

        bool is_pattern;  // true if the value should be an address to the pattern
        bool is_relative; // true if the byte value should be changed by a relative offset
        bool is_address;  // true if the value of the byte should be taken from original_address + offset

        // NOTE: if is_relative and is_address are both true,
        // the value of the byte will be taken from original_address + offset + value

        uint8_t value; // value to change the byte to
        int8_t offset; // offset to add to the value

        std::string pattern; // pattern to calculate the offset from

        byte_t()
        {
            any_byte = false;
            is_pattern = false;
            is_relative = false;
            is_address = false;
            value = 0;
            offset = 0;
        }
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
        bool any_byte;           // true if the byte is a wildcard
        uint8_t byte;            // value of the byte
        bool set_address_cursor; // true if the address cursor should be set to this byte
        bool multi_pattern;      // true if the pattern should be matched multiple times
        int8_t jump_if_fail;     // valid inside [] brackets, tells how many tokens to skip in the pattern if the match fails

        token_t()
        {
            any_byte = false;
            byte = 0;
            set_address_cursor = false;
            multi_pattern = false;
            jump_if_fail = -1;
        }
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
#pragma once
#include <string>

namespace string_utils {
    inline char32_t to_upper(char32_t c) {
        if (c >= U'a' && c <= U'z') return c - 32;
        if (c >= U'а' && c <= U'я') return c - 32;
        if (c == U'ё') return U'Ё';
        return c;
    }

    inline char32_t to_lower(char32_t c) {
        if (c >= U'A' && c <= U'Z') return c + 32;
        if (c >= U'А' && c <= U'Я') return c + 32;
        if (c == U'Ё') return U'ё';
        return c;
    }

    inline uint64_t str_hash(std::u32string const& str) {
        uint64_t hash = 5381;
        for (char32_t c : str)
            hash = ((hash << 5) + hash) + c;
        return hash;
    }

    std::string to_upper(std::string const& str);
    std::string to_lower(std::string const& str);
    std::u32string utf8_to_u32(const std::string& input);
    std::string u32_to_utf8(const std::u32string& input);
};
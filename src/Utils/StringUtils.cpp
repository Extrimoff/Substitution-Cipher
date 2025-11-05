#include "StringUtils.hpp"

#include <algorithm>
#include <utfcpp/utf8.h>

namespace string_utils{
    std::u32string utf8_to_u32(const std::string& input) {
        std::u32string result;
        utf8::utf8to32(input.begin(), input.end(), std::back_inserter(result));
        return result;
    }

    std::string u32_to_utf8(const std::u32string& input) {
        std::string result;
        utf8::utf32to8(input.begin(), input.end(), std::back_inserter(result));
        return result;
    }

    std::string to_upper(std::string const& str) {
        auto res = utf8_to_u32(str);
        std::transform(res.begin(), res.end(), res.begin(), 
            static_cast<char32_t(*)(char32_t)>(to_upper));
        return u32_to_utf8(res);

    }

    std::string to_lower(std::string const& str) {
        auto res = utf8_to_u32(str);
        std::transform(res.begin(), res.end(), res.begin(),
            static_cast<char32_t(*)(char32_t)>(to_lower));
        return u32_to_utf8(res);
    }
}
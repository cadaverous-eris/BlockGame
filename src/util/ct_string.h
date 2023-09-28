#pragma once

#include <string>
#include <string_view>
#include <array>

namespace eng {

    // a constexpr-friendly string that can be used for non-type template arguments
    template <typename C, size_t S>
    struct ct_string {
        using char_type = C;

        std::array<char_type, S> chars = {};

        constexpr ct_string() noexcept = default;
        constexpr ct_string(const char_type (&cstr)[S + 1]) noexcept {
            for (size_t i = 0; i < S; i++)
                chars[i] = cstr[i];
        }

        constexpr operator std::basic_string_view<char_type>() const noexcept {
            return std::basic_string_view<char_type>(chars.data(), S);
        }
        explicit operator std::basic_string<char_type>() const noexcept {
            return std::basic_string<char_type>(chars.data(), S);
        }

        constexpr size_t size() const noexcept { return S; }
        constexpr size_t capacity() const noexcept { return S; }
        constexpr const char_type* data() const noexcept { return chars.data(); }

        constexpr const char_type& operator [](size_t i) const noexcept { return chars[i]; }

        constexpr const char_type* begin() const noexcept { return &chars[0]; }
        constexpr const char_type* cbegin() const noexcept { return &chars[0]; }
        constexpr const char_type* end() const noexcept { return &chars[S]; }
        constexpr const char_type* cend() const noexcept { return &chars[S]; }
    };
    template <typename C, size_t S>
    ct_string(const C (&)[S]) -> ct_string<C, S - 1>;

}
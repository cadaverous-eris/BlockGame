#pragma once

namespace eng {

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template<typename... Ts>
    struct Visitor : Ts... {
        Visitor(const Ts&... args) : Ts(args)... {}
        using Ts::operator()...;
    };

    template<typename... Ts>
    Visitor<Ts...> make_visitor(Ts... lambdas) {
        return Visitor<Ts...>(lambdas...);
    }

}
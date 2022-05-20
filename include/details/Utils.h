#pragma once
#include <type_traits>

namespace decision_tree { namespace details { namespace utils {

    template <typename T, typename Tuple>
    struct has_type;
    template <typename T, typename... Us>
    struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};

    template<typename T>
    struct pass_by_value
    {
        constexpr static auto value = std::is_fundamental_v<T> || std::is_pointer_v<T> || std::is_enum_v<T>;
    };
    template<typename T>
    constexpr inline bool pass_by_value_v = pass_by_value<T>::value;
}}}
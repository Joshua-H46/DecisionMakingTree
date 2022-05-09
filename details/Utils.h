#pragma once
#include <type_traits>

namespace decision_tree { namespace details { namespace utils {

    template <typename T, typename Tuple>
    struct has_type;
    template <typename T, typename... Us>
    struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};
    
}}}
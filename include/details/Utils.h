#pragma once
#include <type_traits>
#include <utility>
#include <cassert>

namespace decision_tree { namespace details { namespace utils {

    // helper class for invalid input
    struct Invalid {};

    // check if a specific type T is in the tuple
    template <typename T, typename Tuple>
    struct has_type;
    template <typename T, typename... Us>
    struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};

    // check if a type T should be passed to functions by value 
    template<typename T>
    struct pass_by_value
    {
        constexpr static auto value = !std::is_class_v<T>;
    };
    template<typename T>
    constexpr inline bool pass_by_value_v = pass_by_value<T>::value;

    // build attribute function type from Class/R if valid, otherwise return Invalid::*
    template<typename Class, typename R, typename Enable = void>
    struct member_attr_func {
        using type = R(Class::*)() const;
    };
    template<typename Class, typename R>
    struct member_attr_func<Class, R, std::enable_if_t<!std::is_class_v<Class>>> {
        using type = void(Invalid::*)() const;
    };
    template<typename Class, typename R>
    using member_attr_func_t = typename member_attr_func<Class, R>::type;

    // get class type from member function
    template<typename MemFunc>
    struct class_for_mem_func;
    template<typename R, typename Class, typename ...Args>
    struct class_for_mem_func<R(Class::*)(Args... args)> {
        using type = Class;
    };
    template<typename R, typename Class, typename ...Args>
    struct class_for_mem_func<R(Class::*)(Args... args) const> {
        using type = Class;
    };
    template<typename MemFunc>
    using class_for_mem_func_t = typename class_for_mem_func<MemFunc>::type;
    
    // helper to call func with args
    template<typename R, typename ...FuncArgs, typename ...Args>
    R proxy_call(R (* func)(FuncArgs...), Args&&... args) {
        return func(std::forward<Args>(args)...);
    }

    // helper to call func with t.attr() and args
    template<typename R, typename T, typename AttrFunc, typename ...FuncArgs, typename ...Args>
    R proxy_call_with_attribute(R (* func)(FuncArgs...), const T& t, AttrFunc attr, Args&&... args) {
        static_assert(std::is_member_function_pointer_v<AttrFunc>);
        if constexpr (std::is_class_v<T>) {
            return func((t.*attr)(), std::forward<Args>(args)...);
        }
        else {
            assert(false && "Should never be here");
            return R{};
        }
    }

}}}
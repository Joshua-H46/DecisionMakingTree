#pragma once
#include <type_traits>
#include <utility>
#include <cassert>

namespace decision_tree { namespace details { namespace utils {

    // helper class for invalid input
    struct Invalid {};
    // helper class for sfinae true type
    template<typename>
    struct sfinae_true : std::true_type {};

    /*  
        Check if a specific type T is in the tuple
    */
    template <typename T, typename Tuple>
    struct has_type;
    template <typename T, typename... Us>
    struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};

    /*
        append type to tuple types
    */
    template<typename T, typename Tuple>
    struct append_tuple;
    template<typename T, template<typename ...Args> typename Tuple, typename ...Args>
    struct append_tuple<T, Tuple<Args...>> {
        using type = Tuple<Args..., T>;
    };
    template<typename T, typename Tuple>
    using append_tuple_t = typename append_tuple<T, Tuple>::type;

    using a = append_tuple_t<int, std::tuple<char, double>>;

    /* 
        a compile time switch case
    */
    template<bool B, typename T>
    struct Cond {
        static constexpr bool value = B;
        using type = T;
    };
    template<typename... Args>
    struct select;
    template<typename Head, typename... Args>
    struct select<Head, Args...> {
        using type = typename std::conditional_t<Head::value, typename Head::type, typename select<Args...>::type>;
    };
    template<typename T>
    struct select<T> {
        using type = T;
    };
    template<typename... Args>
    using select_t = typename select<Args...>::type;

    /*
        Check if a type is a container (with some helper)
    */
    template<typename T>
    auto has_const_iterator_helper(double) -> std::false_type;
    template<typename T>
    auto has_const_iterator_helper(int) -> sfinae_true<typename T::const_iterator*>;
    template<typename T>
    struct has_const_iterator : decltype(has_const_iterator_helper<T>(0)) {};

    template<typename T>
    auto has_begin_helper(double) -> std::false_type;
    template<typename T>
    auto has_begin_helper(int) -> sfinae_true<decltype(std::declval<T>().begin())>;
    template<typename T>
    struct has_begin : decltype(has_begin_helper<T>(0)) {};

    template<typename T>
    auto has_end_helper(double) -> std::false_type;
    template<typename T>
    auto has_end_helper(int) -> sfinae_true<decltype(std::declval<T>().end())>;
    template<typename T>
    struct has_end : decltype(has_end_helper<T>(0)) {};

    template<typename T>
    struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin<T>::value && has_end<T>::value>
    {};

    /*
        get containee from container
    */
    template<typename T, typename Enable = void>
    struct get_containee {
        using type = void;
    };
    template<template<typename... Args> typename Container, typename T, typename... Args>
    struct get_containee<Container<T, Args...>, std::enable_if_t<is_container<Container<T, Args...>>::value>> {
        static_assert(is_container<Container<T, Args...>>::value, "should be container");
        using type = T;
    };
    template<typename T>
    using get_containee_t = typename get_containee<T>::type;

    /*
        build attribute function type from Class/R if valid, otherwise return Invalid::*
    */
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

    /*
        get class type from member function
    */
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
    
    /*
        helper to call func with args
    */
    template<typename R, typename ...FuncArgs, typename ...Args>
    R proxy_call(R (* func)(FuncArgs...), Args&&... args) {
        return func(std::forward<Args>(args)...);
    }

    /*
        helper to call func with t.attr() and args
    */
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
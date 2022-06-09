#pragma once
#include "Utils.h"
#include <type_traits>

namespace decision_tree { namespace details {
    
    enum class CheckType {
        AttributeCheck,         // compare attribute with target of the same type
        RangeCheck,             // compare attribute with target which is range<attr_type> (like vector)
        CustomCheck             // compare by a custom comparator which takes CheckT directly
    };

    enum class PassBy {
        Default,
        Value,
        ConstValue,
        Reference,
        ConstRef
    };

    namespace utils {
        template<typename T, PassBy P>
        constexpr static bool passByRef() {
            return P == PassBy::ConstRef || P == PassBy::Reference || (P == PassBy::Default && std::is_class_v<T>);
        }
        
        template<typename T, PassBy P>
        constexpr static bool passByConst() {
            return P == PassBy::ConstRef || P == PassBy::ConstValue || (P == PassBy::Default && std::is_class_v<T>);
        }
        
        template<typename T, PassBy P>
        struct PassByItem {
            using type = T;
            constexpr static PassBy value = P;
        };

        template<typename ...Args>
        struct PassByRef_helper;
        template<typename Head>
        struct PassByRef_helper<Head> {
            using type = std::conditional_t<utils::passByRef<typename Head::type, Head::value>(),
                                            std::tuple<typename Head::type>, std::tuple<>>;
        };
        template<typename Head, typename ...Args>
        struct PassByRef_helper<Head, Args...> {
            using type = std::conditional_t<utils::passByRef<typename Head::type, Head::value>(),
                                            utils::append_tuple_t<typename Head::type, typename PassByRef_helper<Args...>::type>, typename PassByRef_helper<Args...>::type>;
        };

        template<typename ...Args>
        struct PassByConst_helper;
        template<typename Head>
        struct PassByConst_helper<Head> {
            using type = std::conditional_t<utils::passByConst<typename Head::type, Head::value>(),
                                            std::tuple<typename Head::type>, std::tuple<>>;
        };
        template<typename Head, typename ...Args>
        struct PassByConst_helper<Head, Args...> {
            using type = std::conditional_t<utils::passByConst<typename Head::type, Head::value>(),
                                            utils::append_tuple_t<typename Head::type, typename PassByConst_helper<Args...>::type>, typename PassByConst_helper<Args...>::type>;
        };
    }
}}
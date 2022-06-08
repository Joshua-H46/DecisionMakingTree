#pragma once
#include "MetaData.h"
#include "Utils.h"
#include <cassert>
#include <algorithm>

namespace decision_tree { namespace details { namespace comp {

    namespace Operator {
        struct Greater {};
        struct GreaterEqual {};
        struct Less {};
        struct LessEqual {};
        struct Equal {};
        struct In{};
        struct NotIn{};
    }

    // comparator for the same type
    template<typename MetaData, typename T, typename Op>
    bool _compare(typename MetaData::template ArgType<T> t1_, typename MetaData::template ArgType<T> t2_) {
        static_assert(utils::has_type<Op, std::tuple<Operator::Equal, Operator::Greater, Operator::GreaterEqual, Operator::Less, Operator::LessEqual>>::value);
        if constexpr (std::is_same_v<Op, Operator::Equal>) {
            return t1_ == t2_;
        }
        if constexpr (std::is_same_v<Op, Operator::Greater>) {
            return t1_ > t2_;
        }
        if constexpr (std::is_same_v<Op, Operator::GreaterEqual>) {
            return t1_ >= t2_;
        }
        if constexpr (std::is_same_v<Op, Operator::Less>) {
            return t1_ < t2_;
        }
        if constexpr (std::is_same_v<Op, Operator::LessEqual>) {
            return t1_ <= t2_;
        }
        else {
            return false;
        }
    }

    // comparator for object and range
    template<typename MetaData, typename T1, typename T2, typename Op>
    bool _compare(typename MetaData::template ArgType<T1> t1_, typename MetaData::template ArgType<T2> t2_) {
        static_assert(utils::has_type<Op, std::tuple<Operator::In, Operator::NotIn>>::value);
        static_assert(utils::is_container<T2>::value && std::is_same_v<utils::get_containee_t<T2>, T1>);
        auto&& it = std::find(t2_.begin(), t2_.end(), t1_);
        if constexpr (std::is_same_v<Op, Operator::In>) {
            return it != t2_.end();
        }
        else if constexpr (std::is_same_v<Op, Operator::NotIn>) {
            return it == t2_.end();
        }
        else {
            return false;
        }
    }

    template<typename MetaData, typename T1, typename T2, typename Op>
    bool compare(typename MetaData::template ArgType<T1> t1_, typename MetaData::template ArgType<T2> t2_) {
        static_assert(std::is_same_v<T1, T2> || (utils::is_container<T2>::value && std::is_same_v<utils::get_containee_t<T2>, T1>));
        if constexpr (std::is_same_v<T1, T2>) {
            return _compare<MetaData, T1, Op>(t1_, t2_);
        }
        else if constexpr (utils::is_container<T2>::value && std::is_same_v<utils::get_containee_t<T2>, T1>) {
            return _compare<MetaData, T1, T2, Op>(t1_, t2_);
        }
        else {
            assert(false && "Should not be here");
            return false;
        }
    }
}}}
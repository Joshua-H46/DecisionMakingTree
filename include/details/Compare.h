#pragma once
#include <cassert>

namespace decision_tree { namespace details { namespace comp {

    namespace Operator {
        struct Greater {};
        struct GreaterEqual {};
        struct Less {};
        struct LessEqual {};
        struct Equal {};
    }

    template<typename T, typename Op>
    bool compare(const T& t1_, const T& t2_) {
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
            assert(false && "Should never be here");
            return false;
        }
    }
}}}
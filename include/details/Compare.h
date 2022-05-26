#pragma once
#include <cassert>

namespace decision_tree { namespace details { namespace comp {

    enum class Op {
        Invalid,
        Greater,
        Less,
        GreaterEqual,
        LessEqual,
        Equal,
        In,
        NotIn
    };

    template<typename T>
    bool compare(const T& t1_, const T& t2_, Op op_) {
        switch (op_)
        {
        case Op::Equal:
            return t1_ == t2_;
        case Op::Greater:
            return t1_ > t2_;
        case Op::GreaterEqual:
            return t1_ >= t2_;
        case Op::Less:
            return t1_ < t2_;
        case Op::LessEqual:
            return t1_ <= t2_;
        default:
            assert(false && "Op not supported");
            break;
        }
        return false;
    }

    template<typename T, template<typename _T> typename Range>
    bool compare(const T& t_, const Range<T>& range_, Op op_) {
        auto&& iter = range_.find(t_);
        switch (op_)
        {
        case Op::In:
            return iter != range_.end();
        case Op::NotIn:
            return iter == range_.end();
        default:
            assert(false && "Op not supported");
            break;
        }
        return false;
    }
}}}
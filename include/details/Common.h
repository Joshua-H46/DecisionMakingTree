#pragma once

namespace decision_tree { namespace details {
    
    enum class CheckType {
        AttributeCheck,         // compare attribute with target of the same type
        RangeCheck,             // compare attribute with target which is range<attr_type> (like vector)
        CustomCheck             // compare by a custom comparator which takes CheckT directly
    };
}}
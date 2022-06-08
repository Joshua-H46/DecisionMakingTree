#pragma once
#include "../include/details/MetaData.h"

struct Data 
{
    int id;
    int ival = 2;
    double dval = 2.5;
    std::string sval = "ass";

    int getI() const { return ival; }
    double getD() const { return dval; }
    const std::string& getS() const { return sval; }
};

static bool checkInt(const Data& d, int q2)
{
    return d.getI() <= q2;
}

static bool checkDouble(const Data& d, double p2)
{
    return d.getD() < p2;
}

static bool checkString(const Data& d, const std::string& s)
{
    return s.size() <= d.getS().size();
}

static bool checkIntP(const Data& d, int* p2)
{
    if (p2 == nullptr)
        return false;
    return d.getD() <= *p2;
}
    
RegisterMetaType(Test, Data,
    ((char,         CHAR,           ))
    ((int,          INT,            ))
    ((double,       DOUBLE,         ))
    ((std::string,  STRING,         [](const std::string& s1, const std::string& s2) { return s1 == s2; }))
    ((std::vector<int>,     INT_VEC,    ))
);


namespace decision_tree { namespace details {

/*  The above macro will build a class TestMetaData as below, which can be used as template parameter for DecisionTree
    The first argument is for the class Name, the class would be named "TestMetaName" given "Test" as 1st parameter
    The second argument indicates the object that we apply checks to. For now we pass the object by const ref when applying
    The third argument is a sequence of tuple registering all available types for checks and corresponding enum name.
    The third item in each tuple is optional. We can give our own version of equal comparator for each type. std::equal_to() is used by default. Lambda expression is recommanded here
*/

/*    struct TestMetaData
    {
        // enum of all supported types
        enum class ParamType
        {
            VOID = -1,          // intend for an invalid value
            CHAR,
            INT,
            DOUBLE,
            STRING
        };

        // a tuple type made up of all supported types (can be used for static check)
        using AllTypes = std::tuple<char, int, double, std::string>;
        // a constexpr tuple of equal predicate for each type
        constexpr static auto CompPreds = boost::hana::make_tuple(boost::hana::make_pair(ParamType::CHAR, [](char c1, char c2) {
            return c1 = c2;
        }),
        boost::hana::make_pair(ParamType::INT, [](int i1, int i2) {
            return i1 == i2;
        }),
        boost::hana::make_pair(ParamType::DOUBLE, [](double d1, double d2) {
            return d1 == d2;
        }),
        boost::hana::make_pair(ParamType::STRING, std::equal_to()));

        // convert const type enum to type (specializations are outside the class body)
        template<ParamType t>
        struct getType {
            using type = void;
        };

        // convert types to type enum
        template<typename Type>
        static constexpr ParamType getTypeName() {
            if constexpr (std::is_same_v<Type, char>) {
                return ParamType::CHAR;
            }
            if constexpr (std::is_same_v<Type, int>) {
                return ParamType::INT;
            }
            if constexpr (std::is_same_v<Type, double>) {
                return ParamType::DOUBLE;
            }
            if constexpr (std::is_same_v<Type, std::string>) {
                return ParamType::STRING;
            }
            return ParamType::VOID;
        }

        template<typename T>
        struct passByValue {
            static constexpr bool value = !std::is_class_v<T>;
        };
        template<typename T>
        using ArgType = std::conditional_t<passByValue<T>::value, T, const T&>;
        
        // get CheckT from Register macro. This is the type of objects that need to be checked
        using CheckT = Data;
        using CheckTArg = ArgType<CheckT>;
    };

    // specializations for MetaData::getType
    template<>
    struct TestMetaData::getType<TestMetaData::ParamType::CHAR> {
        using type = char;
    };
    template<>
    struct TestMetaData::getType<TestMetaData::ParamType::INT> {
        using type = int;
    };
    template<>
    struct TestMetaData::getType<TestMetaData::ParamType::DOUBLE> {
        using type = double;
    };
    template<>
    struct TestMetaData::getType<TestMetaData::ParamType::STRING> {
        using type = std::string;
    };

    template<>
    struct MetaDataUtil<TestMetaData>
    {
        static bool is_same(const ConditionCheck<TestMetaData>* c1, const ConditionCheck<TestMetaData>* c2) {
            if (c1->_type != c2->_type || c1->_targetType != c2->_targetType || c1->_checker != c2->_checker || c1->_attr != c2->_attr)
                return false;
            switch (c1->_targetType)
            {
                case TestMetaData::ParamType::CHAR:
                {
                    using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                    return (*((type*)c1->_data)) == (*((type*)c2->_data));
                }
                case TestMetaData::ParamType::INT:
                {
                    using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                    return boost::hana::second(boost::hana::at_c<static_cast<int>(TestMetaData::ParamType::INT)>(TestMetaData::CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));
                    return (*((type*)c1->_data)) == (*((type*)c2->_data));
                }
                case TestMetaData::ParamType::DOUBLE:
                {
                    using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                    return boost::hana::second(boost::hana::at_c<static_cast<int>(TestMetaData::ParamType::DOUBLE)>(TestMetaData::CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));
                    return (*((type*)c1->_data)) == (*((type*)c2->_data));
                }
                case TestMetaData::ParamType::STRING:
                {
                    using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                    return boost::hana::second(boost::hana::at_c<static_cast<int>(TestMetaData::ParamType::STRING)>(TestMetaData::CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));
                    return (*((type*)c1->_data)) == (*((type*)c2->_data));
                }
            }
            return false;
        }

        template<typename Target>
        static ConditionCheck<TestMetaData>* buildCheck(bool(*checker_)(typename TestMetaData::CheckTArg, typename TestMetaData::template ArgType<Target>), const Target& target_)
        {
            auto check = new _ConditionCheck<TestMetaData, std::decay_t<Target>, CheckType::CustomCheck>();
            check->_targetType = TestMetaData::getTypeName<Target>();
            check->_checker = checker_;
            check->_data = target_;
            return (ConditionCheck<TestMetaData>*)check;
        }

        // buildCompare for attribute check
        template<typename Target, typename Op>
        static ConditionCheck<TestMetaData>* buildCompare(details::utils::member_attr_func_t<TestMetaData::CheckT, typename TestMetaData::ArgType<Target>> attr_, const Target& target_, Op)
        {
            auto check = new _ConditionCheck<TestMetaData, Target, CheckType::AttributeCheck>();
            check->_targetType = TestMetaData::getTypeName<Target>();
            check->_attr = attr_;
            check->_checker = comp::compare<TestMetaData, Target, Target, Op>;
            check->_data = target_;
            return (ConditionCheck<TestMetaData>*)check;
        }

        // buildCompare for range check
        template<typename Attribute, template<typename... Args> typename Container, typename Op, typename ...Args>
        static ConditionCheck<TestMetaData>* buildCompare(details::utils::member_attr_func_t<TestMetaData::CheckT, typename TestMetaData::ArgType<Attribute>> attr_, const Container<Attribute, Args...>& target_, Op) 
        {
            auto check = new _ConditionCheck<TestMetaData, Container<Attribute, Args...>, CheckType::RangeCheck>();
            check->_targetType = TestMetaData::getTypeName<Container<Attribute, Args...>>();
            check->_attr = attr_;
            check->_checker = &comp::compare<TestMetaData, Attribute, Container<Attribute, Args...>, Op>;
            check->_data = target_;
            return (ConditionCheck<TestMetaData>*)check;
        }

        template<typename Target, CheckType Type>
        static ConditionCheck<TestMetaData>* copy_impl(ConditionCheck<TestMetaData>* check_) {
            if constexpr (Type != CheckType::RangeCheck || utils::is_container<Target>::value)
            {
                auto in_check = new _ConditionCheck<TestMetaData, Target, Type>();
                in_check->_targetType = check_->_targetType;
                in_check->_attr = (typename _ConditionCheck<TestMetaData, Target, Type>::AttrFunc)check_->_attr;
                in_check->_checker = (typename _ConditionCheck<TestMetaData, Target, Type>::Checker)check_->_checker;
                in_check->_data = *((Target*)check_->_data);
                return (ConditionCheck<TestMetaData>*)in_check;
            }
            else
            {
                assert(false && "Target should be container for range check");
                return nullptr;
            }
        }

        static ConditionCheck<TestMetaData>* copy(ConditionCheck<TestMetaData>* check_)
        {
            switch (check_->_targetType)
            {
            case TestMetaData::ParamType::CHAR:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return copy_impl<type, CheckType::CustomCheck>(check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return copy_impl<type, CheckType::AttributeCheck>(check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return copy_impl<type, CheckType::RangeCheck>(check_);
                }
                break;
            }
            case TestMetaData::ParamType::INT:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return copy_impl<type, CheckType::CustomCheck>(check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return copy_impl<type, CheckType::AttributeCheck>(check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return copy_impl<type, CheckType::RangeCheck>(check_);
                }
                break;
            }
            case TestMetaData::ParamType::DOUBLE:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return copy_impl<type, CheckType::CustomCheck>(check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return copy_impl<type, CheckType::AttributeCheck>(check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return copy_impl<type, CheckType::RangeCheck>(check_);
                }
                break;
            }
            case TestMetaData::ParamType::STRING:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return copy_impl<type, CheckType::CustomCheck>(check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return copy_impl<type, CheckType::AttributeCheck>(check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return copy_impl<type, CheckType::RangeCheck>(check_);
                }
                break;
            }
            }
            return nullptr;
        }

        template<typename Target, CheckType Type>
        static void freeCheck_impl(ConditionCheck<TestMetaData>* check_) 
        {
            if constexpr (Type != CheckType::RangeCheck || utils::is_container<Target>::value)
            {
                delete (_ConditionCheck<TestMetaData, Target, Type>*) check_;
            }
        }
        static void freeCheck(ConditionCheck<TestMetaData>* check_)
        {
            switch (check_->_targetType) {
                case TestMetaData::ParamType::CHAR:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, char>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                    if (check_->_type == CheckType::CustomCheck) {
                        freeCheck_impl<type, CheckType::CustomCheck>(check_);
                    }
                    else if (check_->_type == CheckType::AttributeCheck) {
                        freeCheck_impl<type, CheckType::AttributeCheck>(check_);
                    }
                    else if (check_->_type == CheckType::RangeCheck) {
                        freeCheck_impl<type, CheckType::RangeCheck>(check_);
                    }
                    break;
                }
                case TestMetaData::ParamType::DOUBLE:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, double>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                    if (check_->_type == CheckType::CustomCheck) {
                        freeCheck_impl<type, CheckType::CustomCheck>(check_);
                    }
                    else if (check_->_type == CheckType::AttributeCheck) {
                        freeCheck_impl<type, CheckType::AttributeCheck>(check_);
                    }
                    else if (check_->_type == CheckType::RangeCheck) {
                        freeCheck_impl<type, CheckType::RangeCheck>(check_);
                    }
                    break;
                }
                case TestMetaData::ParamType::INT:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, int>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                    if (check_->_type == CheckType::CustomCheck) {
                        freeCheck_impl<type, CheckType::CustomCheck>(check_);
                    }
                    else if (check_->_type == CheckType::AttributeCheck) {
                        freeCheck_impl<type, CheckType::AttributeCheck>(check_);
                    }
                    else if (check_->_type == CheckType::RangeCheck) {
                        freeCheck_impl<type, CheckType::RangeCheck>(check_);
                    }
                    break;
                }
                case TestMetaData::ParamType::STRING:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, std::string>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                    if (check_->_type == CheckType::CustomCheck) {
                        freeCheck_impl<type, CheckType::CustomCheck>(check_);
                    }
                    else if (check_->_type == CheckType::AttributeCheck) {
                        freeCheck_impl<type, CheckType::AttributeCheck>(check_);
                    }
                    else if (check_->_type == CheckType::RangeCheck) {
                        freeCheck_impl<type, CheckType::RangeCheck>(check_);
                    }
                    break;
                }
            }
        }

        template<typename Target, CheckType Type>
        static bool applyCheck_impl(typename TestMetaData::CheckTArg t_, ConditionCheck<TestMetaData>* check_)
        {
            if constexpr (Type != CheckType::RangeCheck || utils::is_container<Target>::value)
            {
                using Checker = typename _ConditionCheck<TestMetaData, Target, Type>::Checker;
                if constexpr (Type == CheckType::CustomCheck)
                {
                    return details::utils::proxy_call((Checker)check_->_checker, t_, *((Target*)check_->_data));
                }
                else
                {
                    assert(check_->_attr != nullptr && "AttrFunc must not be null");
                    using Checker = typename _ConditionCheck<TestMetaData, Target, Type>::Checker;
                    auto attrfunc = (typename _ConditionCheck<TestMetaData, Target, Type>::AttrFunc)check_->_attr;
                    return details::utils::proxy_call_with_attribute((Checker)check_->_checker, t_, attrfunc, *((Target*)check_->_data));
                }
            }
            else
            {
                assert(false && "Target should be container for range check");
                return false;
            }
        }

        static bool applyCheck(typename TestMetaData::CheckTArg t_, ConditionCheck<TestMetaData>* check_)
        {
            switch (check_->_targetType)
            {
            case TestMetaData::ParamType::CHAR:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return applyCheck_impl<type, CheckType::CustomCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return applyCheck_impl<type, CheckType::AttributeCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return applyCheck_impl<type, CheckType::RangeCheck>(t_, check_);
                }
                break;
            }
            case TestMetaData::ParamType::DOUBLE:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return applyCheck_impl<type, CheckType::CustomCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return applyCheck_impl<type, CheckType::AttributeCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return applyCheck_impl<type, CheckType::RangeCheck>(t_, check_);
                }
                break;
            }
            case TestMetaData::ParamType::INT:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return applyCheck_impl<type, CheckType::CustomCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return applyCheck_impl<type, CheckType::AttributeCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return applyCheck_impl<type, CheckType::RangeCheck>(t_, check_);
                }
                break;
            }
            case TestMetaData::ParamType::STRING:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                if (check_->_type == CheckType::CustomCheck) {
                    return applyCheck_impl<type, CheckType::CustomCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::AttributeCheck) {
                    return applyCheck_impl<type, CheckType::AttributeCheck>(t_, check_);
                }
                else if (check_->_type == CheckType::RangeCheck) {
                    return applyCheck_impl<type, CheckType::RangeCheck>(t_, check_);
                }
                break;
            }
            }
            return false;
        }
    };*/
}}
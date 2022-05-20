#pragma once
// #include "../include/details/MetaData.h"
#include <DecisionTree/details/MetaData.h>

enum Tint
{
    INT = 4
};
    
RegisterMetaType(Test, int,
    ((char,         CHAR))
    ((int,          INT))
    ((double,       DOUBLE))
    ((std::string,  STRING,         [](const std::string& s1, const std::string& s2) { return s1 == s2; }))
    ((Tint,         TINT))
    ((int*,         INTP,           [](const int* p1, const int* p2) { return p1 != nullptr && p2 != nullptr && *p1 == *p2; }))
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
        // get CheckT from Register macro. This is the type of objects that need to be checked
        using CheckT = int;

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
        template<typename T>
        struct getTypeName {
            constexpr static auto value = ParamType::VOID;
        };
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

    // // specializations for getTypeName
    template<>
    struct TestMetaData::getTypeName<char> {
        constexpr static auto value = TestMetaData::ParamType::CHAR;
    };
    template<>
    struct TestMetaData::getTypeName<int> {
        constexpr static auto value = TestMetaData::ParamType::INT;
    };
    template<>
    struct TestMetaData::getTypeName<double> {
        constexpr static auto value = TestMetaData::ParamType::DOUBLE;
    };
    template<>
    struct TestMetaData::getTypeName<std::string> {
        constexpr static auto value = TestMetaData::ParamType::STRING;
    };

    template<>
    struct MetaDataUtil<TestMetaData>
    {
        static bool is_same(const ConditionCheck<TestMetaData>* c1, const ConditionCheck<TestMetaData>* c2) {
            if (c1->_type != c2->_type || c1->_checker != c2->_checker)
                return false;
            switch (c1->_type)
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
        static ConditionCheck<TestMetaData>* buildCheck(bool(*checker_)(const typename TestMetaData::CheckT&, std::conditional_t<std::is_compound_v<Target> && !std::is_enum_v<Target>, const Target&, Target>), const Target& target_)
        {
            auto check = new _ConditionCheck<TestMetaData, std::decay_t<Target>>();
            check->_type = TestMetaData::getTypeName<Target>::value;
            check->_checker = checker_;
            check->_data = target_;
            return (ConditionCheck<TestMetaData>*)check;
        }

        static ConditionCheck<TestMetaData>* copy(ConditionCheck<TestMetaData>* check_)
        {
            switch (check_->_type)
            {
            case TestMetaData::ParamType::CHAR:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                auto in_check = new _ConditionCheck<TestMetaData, type>();
                in_check->_type = check_->_type;
                in_check->_checker = (typename _ConditionCheck<TestMetaData, type>::Checker)check_->_checker;
                in_check->_data = *((type*)check_->_data);
                return (ConditionCheck<TestMetaData>*)in_check;
                break;
            }
            case TestMetaData::ParamType::INT:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                auto in_check = new _ConditionCheck<TestMetaData, type>();
                in_check->_type = check_->_type;
                in_check->_checker = (typename _ConditionCheck<TestMetaData, type>::Checker)check_->_checker;
                in_check->_data = *((type*)check_->_data);
                return (ConditionCheck<TestMetaData>*)in_check;
                break;
            }
            case TestMetaData::ParamType::DOUBLE:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                auto in_check = new _ConditionCheck<TestMetaData, type>();
                in_check->_type = check_->_type;
                in_check->_checker = (typename _ConditionCheck<TestMetaData, type>::Checker)check_->_checker;
                in_check->_data = *((type*)check_->_data);
                return (ConditionCheck<TestMetaData>*)in_check;
                break;
            }
            case TestMetaData::ParamType::STRING:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                auto in_check = new _ConditionCheck<TestMetaData, type>();
                in_check->_type = check_->_type;
                in_check->_checker = (typename _ConditionCheck<TestMetaData, type>::Checker)check_->_checker;
                in_check->_data = *((type*)check_->_data);
                return (ConditionCheck<TestMetaData>*)in_check;
                break;
            }
            }
            return nullptr;
        }

        static void freeCheck(ConditionCheck<TestMetaData>* check_)
        {
            switch (check_->_type) {
                case TestMetaData::ParamType::CHAR:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, char>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                    delete (_ConditionCheck<TestMetaData, type>*)check_;
                    break;
                }
                case TestMetaData::ParamType::DOUBLE:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, double>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                    delete (_ConditionCheck<TestMetaData, type>*)check_;
                    break;
                }
                case TestMetaData::ParamType::INT:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, int>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                    delete (_ConditionCheck<TestMetaData, type>*)check_;
                    break;
                }
                case TestMetaData::ParamType::STRING:
                {
                    // std::cout << "Clear _ConditionCheck<CheckT, std::string>\n";
                    using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                    delete (_ConditionCheck<TestMetaData, type>*)check_;
                    break;
                }
            }
        }

        static bool applyCheck(const typename TestMetaData::CheckT& t_, ConditionCheck<TestMetaData>* check_)
        {
            switch (check_->_type)
            {
            case TestMetaData::ParamType::CHAR:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::CHAR>::type;
                using Checker = typename _ConditionCheck<TestMetaData, type>::Checker;
                return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
                break;
            }
            case TestMetaData::ParamType::DOUBLE:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::DOUBLE>::type;
                using Checker = typename _ConditionCheck<TestMetaData, type>::Checker;
                return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
                break;
            }
            case TestMetaData::ParamType::INT:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::INT>::type;
                using Checker = typename _ConditionCheck<TestMetaData, type>::Checker;
                return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
                break;
            }
            case TestMetaData::ParamType::STRING:
            {
                using type = TestMetaData::getType<TestMetaData::ParamType::STRING>::type;
                using Checker = typename _ConditionCheck<TestMetaData, type>::Checker;
                return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
                break;
            }
            }
            return false;
        }
    };
*/
}}
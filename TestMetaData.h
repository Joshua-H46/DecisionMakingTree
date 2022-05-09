#include "details/MetaData.h"

namespace decision_tree { namespace test {
    
    RegisterMetaType(Test, int,
        ((char,         CHAR))
        ((int,          INT))
        ((double,       DOUBLE))
        ((std::string,  STRING,         [](const std::string& s1, const std::string& s2) { return s1 == s2; }))
    );


/*  The above macro will build a class TestMetaData as below, which can be used as template parameter for DecisionTree
    The first argument is for the class Name, the class would be named "TestMetaName" given "Test" as 1st parameter
    The second argument indicates the object that we apply checks to. For now we pass the object by const ref when applying
    The third argument is a sequence of tuple registering all available types for checks and corresponding enum name.
    The third item in each tuple is optional. We can give our own version of equal comparator for each type. std::equal_to() is used by default. Lambda expression is recommanded here

    class TestMetaData
    {
        template<typename MetaData, typename ReturnT>
        friend struct decision_tree::Rule;
        template<typename MetaData, typename ReturnT>
        friend struct decision_tree::details::_Rule;
        template<typename MetaData, typename ReturnT>
        friend class decision_tree::DecisionTree;

        // get CheckT from Register macro. This is the type of objects that need to be checked
        using CheckT = int;

        // enum of all supported types
        enum class TypeName
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
        constexpr static auto CompPreds = boost::hana::make_tuple(boost::hana::make_pair(TypeName::CHAR, [](char c1, char c2) {
            return c1 = c2;
        }),
        boost::hana::make_pair(TypeName::INT, [](int i1, int i2) {
            return i1 == i2;
        }),
        boost::hana::make_pair(TypeName::DOUBLE, [](double d1, double d2) {
            return d1 == d2;
        }),
        boost::hana::make_pair(TypeName::STRING, std::equal_to()));

        // convert const type enum to type (specializations are outside the class body)
        template<TypeName t>
        struct getType {
            using type = void;
        };

        // convert types to type enum
        template<typename T>
        struct getTypeName {
            constexpr static auto value = TypeName::VOID;
        };

        // struct describing a generic Check
        struct Check
        {
            typedef void(*CheckerType)(void);       // a generic function pointer type
            TypeName _type;
            CheckerType _checker;
            char _data[8];                          // a generic data pointer
        };

        // class template that handles different Checks
        template<typename Target, typename Enable = void>
        struct _Check
        {
            static_assert(utils::has_type<Target, AllTypes>::value, "TypeName not supported");
            typedef bool(*Checker)(const CheckT&, const Target&);      // std::decay first?
            TypeName _type;
            Checker _checker;       // checker function with signature bool (EO, Target)
            Target _data;           // the data we use in the check
        };


        // helper function
        static bool is_same(const Check* c1, const Check* c2);     // check if two checks are the same
        template<typename Target>
        static Check* buildCheck(bool(* checker_)(const CheckT&, std::conditional_t<std::is_compound_v<Target>, const Target&, Target>), const Target& target_);
        static Check* copy(Check* check_);
        static void free(Check* c);                               // delete a Check object
        static bool applyCheck(const CheckT& t_, Check* check_);   // do check
    };

    // specializations for MetaData::getType
    template<>
    struct TestMetaData::getType<TestMetaData::TypeName::CHAR> {
        using type = char;
    };
    template<>
    struct TestMetaData::getType<TestMetaData::TypeName::INT> {
        using type = int;
    };
    template<>
    struct TestMetaData::getType<TestMetaData::TypeName::DOUBLE> {
        using type = double;
    };
    template<>
    struct TestMetaData::getType<TestMetaData::TypeName::STRING> {
        using type = std::string;
    };

    // // specializations for getTypeName
    template<>
    struct TestMetaData::getTypeName<char> {
        constexpr static auto value = TestMetaData::TypeName::CHAR;
    };
    template<>
    struct TestMetaData::getTypeName<int> {
        constexpr static auto value = TestMetaData::TypeName::INT;
    };
    template<>
    struct TestMetaData::getTypeName<double> {
        constexpr static auto value = TestMetaData::TypeName::DOUBLE;
    };
    template<>
    struct TestMetaData::getTypeName<std::string> {
        constexpr static auto value = TestMetaData::TypeName::STRING;
    };
    
    // specialization for _Check
    template<typename Target>
    struct TestMetaData::_Check<Target, std::enable_if_t<std::is_fundamental_v<Target> || std::is_pointer_v<Target>>>
    {
        static_assert(utils::has_type<Target, AllTypes>::value, "TypeName not supported");
        typedef bool(*Checker)(const TestMetaData::CheckT&, Target);
        TypeName _type;
        Checker _checker;
        Target _data;
    };

    bool TestMetaData::is_same(const TestMetaData::Check* c1, const TestMetaData::Check* c2) {
        if (c1->_type != c2->_type || c1->_checker != c2->_checker)
            return false;
        switch (c1->_type)
        {
            case TestMetaData::TypeName::CHAR:
            {
                using type = TestMetaData::getType<TestMetaData::TypeName::CHAR>::type;
                return (*((type*)c1->_data)) == (*((type*)c2->_data));
            }
            case TestMetaData::TypeName::INT:
            {
                using type = TestMetaData::getType<TestMetaData::TypeName::INT>::type;
                return boost::hana::second(boost::hana::at_c<static_cast<int>(TestMetaData::TypeName::INT)>(CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));
                return (*((type*)c1->_data)) == (*((type*)c2->_data));
            }
            case TestMetaData::TypeName::DOUBLE:
            {
                using type = TestMetaData::getType<TestMetaData::TypeName::DOUBLE>::type;
                return boost::hana::second(boost::hana::at_c<static_cast<int>(TestMetaData::TypeName::DOUBLE)>(CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));
                return (*((type*)c1->_data)) == (*((type*)c2->_data));
            }
            case TestMetaData::TypeName::STRING:
            {
                using type = TestMetaData::getType<TestMetaData::TypeName::STRING>::type;
                return boost::hana::second(boost::hana::at_c<static_cast<int>(TestMetaData::TypeName::STRING)>(CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));
                return (*((type*)c1->_data)) == (*((type*)c2->_data));
            }
        }
        return false;
    }

    template<typename Target>
    TestMetaData::Check* TestMetaData::buildCheck(bool(* checker_)(const TestMetaData::CheckT&, std::conditional_t<std::is_compound_v<Target>, const Target&, Target>), const Target& target_)
    {
        auto check = new _Check<std::decay_t<Target>>();
        check->_type = getTypeName<Target>::value;
        check->_checker = checker_;
        check->_data = target_;
        return (Check*)check;
    }

    TestMetaData::Check* TestMetaData::copy(TestMetaData::Check* check_)
    {
        switch (check_->_type)
        {
        case TypeName::CHAR:
        {
            using type = getType<TypeName::CHAR>::type;
            auto in_check = new _Check<type>();
            in_check->_type = check_->_type;
            in_check->_checker = (typename _Check<type>::Checker)check_->_checker;
            in_check->_data = *((type*)check_->_data);
            return (Check*)in_check;
            break;
        }
        case TypeName::INT:
        {
            using type = getType<TypeName::INT>::type;
            auto in_check = new _Check<type>();
            in_check->_type = check_->_type;
            in_check->_checker = (typename _Check<type>::Checker)check_->_checker;
            in_check->_data = *((type*)check_->_data);
            return (Check*)in_check;
            break;
        }
        case TypeName::DOUBLE:
        {
            using type = getType<TypeName::DOUBLE>::type;
            auto in_check = new _Check<type>();
            in_check->_type = check_->_type;
            in_check->_checker = (typename _Check<type>::Checker)check_->_checker;
            in_check->_data = *((type*)check_->_data);
            return (Check*)in_check;
            break;
        }
        case TypeName::STRING:
        {
            using type = getType<TypeName::STRING>::type;
            auto in_check = new _Check<type>();
            in_check->_type = check_->_type;
            in_check->_checker = (typename _Check<type>::Checker)check_->_checker;
            in_check->_data = *((type*)check_->_data);
            return (Check*)in_check;
            break;
        }
        }
        return nullptr;
    }

    void TestMetaData::free(TestMetaData::Check* check_)
    {
        switch (check_->_type) {
            case TestMetaData::TypeName::CHAR:
            {
                // std::cout << "Clear _Check<CheckT, char>\n";
                using type = getType<TestMetaData::TypeName::CHAR>::type;
                delete (TestMetaData::_Check<TestMetaData::CheckT, type>*)check_;
                break;
            }
            case TestMetaData::TypeName::DOUBLE:
            {
                // std::cout << "Clear _Check<CheckT, double>\n";
                using type = getType<TestMetaData::TypeName::DOUBLE>::type;
                delete (TestMetaData::_Check<TestMetaData::CheckT, type>*)check_;
                break;
            }
            case TestMetaData::TypeName::INT:
            {
                // std::cout << "Clear _Check<CheckT, int>\n";
                using type = getType<TestMetaData::TypeName::INT>::type;
                delete (TestMetaData::_Check<TestMetaData::CheckT, type>*)check_;
                break;
            }
            case TestMetaData::TypeName::STRING:
            {
                // std::cout << "Clear _Check<CheckT, std::string>\n";
                using type = getType<TestMetaData::TypeName::STRING>::type;
                delete (TestMetaData::_Check<TestMetaData::CheckT, type>*)check_;
                break;
            }
        }
    }

    bool TestMetaData::applyCheck(const TestMetaData::CheckT& t_, TestMetaData::Check* check_)
    {
        switch (check_->_type)
        {
        case TestMetaData::TypeName::CHAR:
        {
            using type = getType<TestMetaData::TypeName::CHAR>::type;
            using Checker = typename TestMetaData::_Check<type>::Checker;
            return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
            break;
        }
        case TestMetaData::TypeName::DOUBLE:
        {
            using type = getType<TestMetaData::TypeName::DOUBLE>::type;
            using Checker = typename TestMetaData::_Check<type>::Checker;
            return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
            break;
        }
        case TestMetaData::TypeName::INT:
        {
            using type = getType<TestMetaData::TypeName::INT>::type;
            using Checker = typename TestMetaData::_Check<type>::Checker;
            return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
            break;
        }
        case TestMetaData::TypeName::STRING:
        {
            using type = getType<TestMetaData::TypeName::STRING>::type;
            using Checker = typename TestMetaData::_Check<type>::Checker;
            return ((Checker)check_->_checker)(t_, *((type*)check_->_data));
            break;
        }
        }
        return false;
    }
*/
}}
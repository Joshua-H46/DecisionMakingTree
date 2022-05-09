#pragma once
#include "Utils.h"
#include <tuple>
#include <functional>
#include <type_traits>
#include <boost/hana.hpp>
#include <boost/preprocessor.hpp>

namespace decision_tree {
    template<typename MetaData, typename DataType>
    struct Rule;
    template<typename MetaData, typename ReturnT>
    class DecisionTree;

    namespace details {
        template<typename MetaData, typename DataType>
        struct _Rule;
    }
}

/*  Some Helper Macros  */
#define AddTemplateT_0()
#define AddTemplateT_1() template<typename T1>
#define AddTemplateT_2() template<typename T1, typename T2>
#define AddTemplateT_3() template<typename T1, typename T2, typename T3>
#define AddFriendClass(NAME, N) AddTemplateT_##N()  friend class NAME;

#define SelectElem_0(_, __, x)     BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(x), 0, x)
#define SelectElems_0(_, __, x)    BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(x), 0, x),
#define SelectElem_1(_, __, x)     BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(x), 1, x)
#define SelectElems_1(_, __, x)    BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(x), 1, x),
#define SelectElem_2(_, __, x)     BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(x), 2, x)
#define SelectElems_2(_, __, x)    BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(x), 2, x),
#define PreviousElems(Seq)      BOOST_PP_IF(BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(Seq), 1), BOOST_PP_SEQ_REMOVE(Seq, BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(Seq), 1)), Seq)
#define SingleElem(Seq, Data, Elem, _)         BOOST_PP_SEQ_FOR_EACH(Elem, Data, Seq)
#define MultiElems(Seq, Data, Elem, Elems)     BOOST_PP_SEQ_FOR_EACH(Elems, Data, PreviousElems(Seq)) Elem(_, Data, BOOST_PP_SEQ_ELEM(BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(Seq), 1), Seq))
#define AllElems(Seq, Data, Elem, Elems)       BOOST_PP_IF(BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(Seq), 1), MultiElems, SingleElem)(Seq, Data, Elem, Elems)

/*  TypeName enum definition    */
#define MetaDataTypeEnumDef(Seq)                \
    enum class TypeName {                       \
        VOID = -1,                              \
        AllElems(Seq, _, SelectElem_1, SelectElems_1)    \
    };

/*  tuple of Comparator for each registered type    */
#define MetaDataMakeCompPair(_, __, x)  boost::hana::make_pair(TypeName::SelectElem_1(_, __, x), BOOST_PP_IF(BOOST_PP_SUB(BOOST_PP_TUPLE_SIZE(x), 2), SelectElem_2(_, __, x), std::equal_to()))
#define MetaDataMakeCompPairs(_, __, x) MetaDataMakeCompPair(_, __, x),
#define MetaDataCompDef(Seq)                    \
    constexpr static auto CompPreds = boost::hana::make_tuple(AllElems(Seq, _, MetaDataMakeCompPair, MetaDataMakeCompPairs));
    
/*  Check definition    */
#define MetaDataCheckDef()                              \
    struct Check                                        \
    {                                                   \
        typedef void(*CheckerType)(void);               \
        TypeName _type;                                 \
        CheckerType _checker;                           \
        char _data[8];                                  \
    };                                                  \
                                                        \
    template<typename Target, typename Enable = void>   \
    struct _Check                                       \
    {                                                   \
        static_assert(decision_tree::details::utils::has_type<Target, AllTypes>::value, "TypeName not supported");      \
        typedef bool(*Checker)(const CheckT&, const Target&);       \
        TypeName _type;                                 \
        Checker _checker;                               \
        Target _data;                                   \
    };                                          

/*  MetaData class body impl    */
#define MetaDataBodyImpl(Type, Seq)                     \
    using CheckT = Type;                                \
    using AllTypes = std::tuple<AllElems(Seq, _, SelectElem_0, SelectElems_0)>;  \
    MetaDataTypeEnumDef(Seq)                            \
    MetaDataCompDef(Seq)                                \
    struct Check                                        \
    {                                                   \
        typedef void(*CheckerType)(void);               \
        TypeName _type;                                 \
        CheckerType _checker;                           \
        char _data[8];                                  \
    };                                                  \
                                                        \
    template<typename Target, typename Enable = void>   \
    struct _Check                                       \
    {                                                   \
        static_assert(decision_tree::details::utils::has_type<Target, AllTypes>::value, "TypeName not supported");      \
        typedef bool(*Checker)(const CheckT&, const Target&);       \
        TypeName _type;                                             \
        Checker _checker;                                           \
        Target _data;                                               \
    };                                                              \
                                                                    \
    template<TypeName t>                                            \
    struct getType {                                                \
        using type = void;                                          \
    };                                                              \
                                                                    \
    template<typename T>                                            \
    struct getTypeName {                                            \
        constexpr static auto value = TypeName::VOID;               \
    };                                                              \
                                                                    \
    static bool is_same(const Check* c1, const Check* c2);          \
    template<typename Target>                                       \
    static Check* buildCheck(bool(* checker_)(const CheckT&, std::conditional_t<std::is_compound_v<Target>, const Target&, Target>), const Target& target_);    \
    static Check* copy(Check* check_);                              \
    static void free(Check* c);                                     \
    static bool applyCheck(const CheckT& t_, Check* check_);            

/* template struct specification  */
#define MetaDataGetTypeSpecImpl(_, CLASS, x)                        \
    template<>                                                      \
    struct CLASS::getType<CLASS::TypeName::SelectElem_1(_, _, x)> {     \
        using type = SelectElem_0(_, _, x);                             \
    };                                                              
#define MetaDataGetTypeNameSpecImpl(_, CLASS, x)                    \
    template<>                                                      \
    struct CLASS::getTypeName<SelectElem_0(_, _, x)> {              \
        constexpr static auto value = CLASS::TypeName::SelectElem_1(_, _, x);       \
    };
#define MetaDataCheckSpecImpl()                                     \
    template<typename Target>                                       \
    struct TestMetaData::_Check<Target, std::enable_if_t<std::is_fundamental_v<Target> || std::is_pointer_v<Target>>>   \
    {                                                               \
        static_assert(decision_tree::details::utils::has_type<Target, AllTypes>::value, "TypeName not supported");      \
        typedef bool(*Checker)(const TestMetaData::CheckT&, Target);\
        TypeName _type;                                             \
        Checker _checker;                                           \
        Target _data;                                               \
    };
#define MetaDataSpecificationImpl(CLASS, Seq)               \
    AllElems(Seq, CLASS, MetaDataGetTypeSpecImpl, MetaDataGetTypeSpecImpl)          \
    AllElems(Seq, CLASS, MetaDataGetTypeNameSpecImpl, MetaDataGetTypeNameSpecImpl)  \
    MetaDataCheckSpecImpl()

/*  MetaData function implementation - mainly switch case   */
#define MetaDataTypeEnumSwitchCase(CLASS, Seq, NormalCase, DefaultCase)     \
    BOOST_PP_SEQ_FOR_EACH(NormalCase, CLASS, Seq)                           \
    DefaultCase(CLASS)

/* bool MetaData::is_same(const Check* c1, const Check* c2) */
#define IsSameNormalCase(_, CLASS, x)                   \
    case CLASS::TypeName::SelectElem_1(_, _, x):              \
    {                                                   \
        using type = CLASS::getType<CLASS::TypeName::SelectElem_1(_, _, x)>::type;   \
        return boost::hana::second(boost::hana::at_c<static_cast<int>(CLASS::TypeName::SelectElem_1(_, _, x))>(CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));      \
    }
#define IsSameDefaultCase(CLASS)
#define MetaDataIsSameImpl(CLASS, TYPE, Seq)    \
    bool CLASS::is_same(const CLASS::Check* c1, const CLASS::Check* c2) {       \
        if (c1->_type != c2->_type || c1->_checker != c2->_checker)             \
            return false;                                                       \
        switch (c1->_type)                                                      \
        {                                                                       \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, IsSameNormalCase, IsSameDefaultCase) \
        }                                                                       \
        return false;                                                           \
    }

/*  template<typename Target>
    TestMetaData::Check* TestMetaData::buildCheck(bool(* checker_)(const TestMetaData::CheckT&, std::conditional_t<std::is_compound_v<Target>, const Target&, Target>), const Target& target_)*/
#define MetaDataBuildCheckImpl(CLASS, TYPE, Seq)            \
    template<typename Target>                               \
    CLASS::Check* CLASS::buildCheck(bool(* checker_)(const CLASS::CheckT&, std::conditional_t<std::is_compound_v<Target>, const Target&, Target>), const Target& target_)  \
    {                                                       \
        auto check = new _Check<std::decay_t<Target>>();    \
        check->_type = getTypeName<Target>::value;          \
        check->_checker = checker_;                         \
        check->_data = target_;                             \
        return (Check*)check;                               \
    }

/*  Check* MetaData::copy(Check* check_)    */
#define MetaDataCopyNormalCase(_, CLASS, x)                 \
    case CLASS::TypeName::SelectElem_1(_, _, x):            \
    {                                                       \
        using type = getType<TypeName::SelectElem_1(_, _, x)>::type;    \
        auto in_check = new _Check<type>();                 \
        in_check->_type = check_->_type;                    \
        in_check->_checker = (typename _Check<type>::Checker)check_->_checker;  \
        in_check->_data = *((type*)check_->_data);          \
        return (Check*)in_check;                            \
        break;                                              \
    }
#define MetaDataCopyDefaultCase(CLASS)
#define MetaDataCopyImpl(CLASS, TYPE, Seq)                  \
    CLASS::Check* CLASS::copy(CLASS::Check* check_)         \
    {                                                       \
        switch (check_->_type)                              \
        {                                                   \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataCopyNormalCase, MetaDataCopyDefaultCase) \
        }                                                   \
        return nullptr;                                     \
    }

/*  void MetaData::free(Check* check_)  */
#define MetaDataFreeNormalCase(_, CLASS, x)                 \
    case CLASS::TypeName::SelectElem_1(_,_,x):              \
    {                                                       \
        using type = getType<TypeName::SelectElem_1(_,_,x)>::type;      \
        delete (_Check<CheckT, type>*)check_;               \
        break;                                              \
    }
#define MetaDataFreeDefaultCase(CLASS)
#define MetaDataFreeImpl(CLASS, TYPE, Seq)                  \
    void TestMetaData::free(CLASS::Check* check_)           \
    {                                                       \
        switch (check_->_type) {                            \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataFreeNormalCase, MetaDataFreeDefaultCase) \
        }                                                   \
    }

/*  bool TestMetaData::applyCheck(const TestMetaData::CheckT& t_, TestMetaData::Check* check_)  */
#define MetaDataApplyNormalCase(_, Class, x)                \
    case TypeName::SelectElem_1(_,_,x):                     \
    {                                                       \
        using type = getType<TypeName::SelectElem_1(_,_,x)>::type;          \
        using Checker = typename _Check<type>::Checker;     \
        return ((Checker)check_->_checker)(t_, *((type*)check_->_data));    \
    }   
#define MetaDataApplyDefaultCase(CLASS)
#define MetaDataApplyImpl(CLASS, TYPE, Seq)                 \
    bool TestMetaData::applyCheck(const TestMetaData::CheckT& t_, TestMetaData::Check* check_)  \
    {                                                       \
        switch (check_->_type)                              \
        {                                                   \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataApplyNormalCase, MetaDataApplyDefaultCase)   \
        }                                                   \
        return false;                                       \
    }
#define MetaDataFuncImpl(CLASS, TYPE, Seq)      \
    MetaDataIsSameImpl(CLASS, TYPE, Seq)        \
    MetaDataBuildCheckImpl(CLASS, TYPE, Seq)    \
    MetaDataCopyImpl(CLASS, TYPE, Seq)          \
    MetaDataApplyImpl(CLASS, TYPE, Seq)         \
    MetaDataFreeImpl(CLASS, TYPE, Seq)


/*  **********************************
    RegisterMetaType:
    Macro to register types that could be used in the check
    Some classes and helper functions would be set also
    Usage:
        RegisterMetaType(Nebula, ExchangeOrder,
            ((int,          INT,            equal_int))
            ((double,       DOUBLE,         equal_double))
            ((std::string,  STRING,         equal_string)));
        A classes named Nebula_MetaData would be constructed
        The third argument in each line defines how to compare two objects of the same type and is optional(std::equal_to by default)
    ************************************    */
#define RegisterMetaType(Name, Type, Seq)       \
class Name ## MetaData {                        \
    AddFriendClass(decision_tree::Rule, 2);                             \
    AddFriendClass(decision_tree::details::_Rule, 2);                   \
    AddFriendClass(decision_tree::DecisionTree, 2);                     \
    MetaDataBodyImpl(Type, Seq);                                        \
};                                                                      \
MetaDataSpecificationImpl(Name ## MetaData, Seq)                        \
MetaDataFuncImpl(Name ## MetaData, Type, Seq)

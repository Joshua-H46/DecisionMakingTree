#pragma once
#include "Utils.h"
#include <tuple>
#include <functional>
#include <type_traits>
#include <boost/hana.hpp>
#include <boost/preprocessor.hpp>

namespace decision_tree { namespace details {
    
    template<typename MetaData>
    struct Check
    {
        typedef void(*CheckerType)(void);       // a generic function pointer type
        typename MetaData::TypeName _type;
        CheckerType _checker;
        char _data[8];                          // a generic data pointer
    };
    // class template that handles different Checks
    template<typename MetaData, typename Target, typename Enable = void>
    struct _Check
    {
        static_assert(decision_tree::details::utils::has_type<Target, typename MetaData::AllTypes>::value, "TypeName not supported");
        typedef bool(*Checker)(const typename MetaData::CheckT&, const Target&);      // std::decay first?
        typename MetaData::TypeName _type;
        Checker _checker;       // checker function with signature bool (EO, Target)
        Target _data;           // the data we use in the check
    };
    // specialization for _Check
    template<typename MetaData, typename Target>
    struct _Check<MetaData, Target, std::enable_if_t<utils::pass_by_value_v<Target>>>
    {
        static_assert(decision_tree::details::utils::has_type<Target, typename MetaData::AllTypes>::value, "TypeName not supported");
        typedef bool(*Checker)(const typename MetaData::CheckT&, Target);
        typename MetaData::TypeName _type;
        Checker _checker;
        Target _data;
    };

    template<typename MetaData>
    struct MetaDataUtil
    {
        // helper function
        static bool is_same(const Check<MetaData>* c1, const Check<MetaData>* c2);     // check if two checks are the same
        template<typename Target>
        static Check<MetaData>* buildCheck(bool(* checker_)(const typename MetaData::CheckT&, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_);
        static Check<MetaData>* copy(Check<MetaData>* check_);
        static void freeCheck(Check<MetaData>* c);                               // delete a Check object
        static bool applyCheck(const typename MetaData::CheckT& t_, Check<MetaData>* check_);   // do check
    };
}}


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
    
/*  MetaData class body impl    */
#define MetaDataBodyImpl(Type, Seq)                                 \
    using CheckT = Type;                                            \
    using AllTypes = std::tuple<AllElems(Seq, _, SelectElem_0, SelectElems_0)>;  \
    MetaDataTypeEnumDef(Seq)                                        \
    MetaDataCompDef(Seq)                                            \
                                                                    \
    template<TypeName t>                                            \
    struct getType {                                                \
        using type = void;                                          \
    };                                                              \
                                                                    \
    template<typename T>                                            \
    struct getTypeName {                                            \
        constexpr static auto value = TypeName::VOID;               \
    };

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

#define MetaDataSpecificationImpl(CLASS, Seq)               \
    AllElems(Seq, CLASS, MetaDataGetTypeSpecImpl, MetaDataGetTypeSpecImpl)          \
    AllElems(Seq, CLASS, MetaDataGetTypeNameSpecImpl, MetaDataGetTypeNameSpecImpl)

/*  MetaDataUtil function implementation - mainly switch case   */
#define MetaDataTypeEnumSwitchCase(CLASS, Seq, NormalCase, DefaultCase)     \
    BOOST_PP_SEQ_FOR_EACH(NormalCase, CLASS, Seq)                           \
    DefaultCase(CLASS)

/* bool MetaDataUtil::is_same(const Check* c1, const Check* c2) */
#define IsSameNormalCase(_, CLASS, x)                   \
    case CLASS::TypeName::SelectElem_1(_, _, x):              \
    {                                                   \
        using type = CLASS::getType<CLASS::TypeName::SelectElem_1(_, _, x)>::type;   \
        return boost::hana::second(boost::hana::at_c<static_cast<int>(CLASS::TypeName::SelectElem_1(_, _, x))>(CLASS::CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));      \
    }
#define IsSameDefaultCase(CLASS)
#define MetaDataUtilIsSameImpl(CLASS, TYPE, Seq)    \
    static bool is_same(const Check<CLASS>* c1, const Check<CLASS>* c2) {       \
        if (c1->_type != c2->_type || c1->_checker != c2->_checker)             \
            return false;                                                       \
        switch (c1->_type)                                                      \
        {                                                                       \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, IsSameNormalCase, IsSameDefaultCase) \
        }                                                                       \
        return false;                                                           \
    }

/*  template<typename Target>
    Check<MetaData>* MetaDataUtil::buildCheck(bool(* checker_)(const MetaData::CheckT&, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_)*/
#define MetaDataUtilBuildCheckImpl(CLASS, TYPE, Seq)        \
    template<typename Target>                               \
    static Check<CLASS>* buildCheck(bool(* checker_)(const typename CLASS::CheckT&, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_)  \
    {                                                       \
        auto check = new _Check<CLASS, std::decay_t<Target>>();  \
        check->_type = CLASS::getTypeName<Target>::value;   \
        check->_checker = checker_;                         \
        check->_data = target_;                             \
        return (Check<CLASS>*)check;                        \
    }

/*  Check* MetaDataUtil::copy(Check* check_)    */
#define MetaDataCopyNormalCase(_, CLASS, x)                 \
    case CLASS::TypeName::SelectElem_1(_, _, x):            \
    {                                                       \
        using type = CLASS::getType<CLASS::TypeName::SelectElem_1(_, _, x)>::type;    \
        auto in_check = new _Check<CLASS, type>();          \
        in_check->_type = check_->_type;                    \
        in_check->_checker = (typename _Check<CLASS, type>::Checker)check_->_checker;  \
        in_check->_data = *((type*)check_->_data);          \
        return (Check<CLASS>*)in_check;                     \
        break;                                              \
    }
#define MetaDataCopyDefaultCase(CLASS)
#define MetaDataUtilCopyImpl(CLASS, TYPE, Seq)              \
    static Check<CLASS>* copy(Check<CLASS>* check_)         \
    {                                                       \
        switch (check_->_type)                              \
        {                                                   \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataCopyNormalCase, MetaDataCopyDefaultCase) \
        }                                                   \
        return nullptr;                                     \
    }

/*  void MetaData::freeCheck(Check* check_)  */
#define MetaDataFreeNormalCase(_, CLASS, x)                 \
    case CLASS::TypeName::SelectElem_1(_,_,x):              \
    {                                                       \
        using type = CLASS::getType<CLASS::TypeName::SelectElem_1(_,_,x)>::type;      \
        delete (_Check<CLASS, type>*)check_;                \
        break;                                              \
    }
#define MetaDataFreeDefaultCase(CLASS)
#define MetaDataUtilFreeImpl(CLASS, TYPE, Seq)              \
    static void freeCheck(Check<CLASS>* check_)             \
    {                                                       \
        switch (check_->_type) {                            \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataFreeNormalCase, MetaDataFreeDefaultCase) \
        }                                                   \
    }

/*  bool MetaData::applyCheck(const MetaData::CheckT& t_, MetaData::Check* check_)  */
#define MetaDataApplyNormalCase(_, CLASS, x)                \
    case CLASS::TypeName::SelectElem_1(_,_,x):                     \
    {                                                       \
        using type = CLASS::getType<CLASS::TypeName::SelectElem_1(_,_,x)>::type;          \
        using Checker = typename _Check<CLASS, type>::Checker;     \
        return ((Checker)check_->_checker)(t_, *((type*)check_->_data));    \
    }   
#define MetaDataApplyDefaultCase(CLASS)
#define MetaDataUtilApplyImpl(CLASS, TYPE, Seq)             \
    static bool applyCheck(const typename CLASS::CheckT& t_, Check<CLASS>* check_)  \
    {                                                       \
        switch (check_->_type)                              \
        {                                                   \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataApplyNormalCase, MetaDataApplyDefaultCase)   \
        }                                                   \
        return false;                                       \
    }

#define MetaDataUtilSpecImpl(CLASS, TYPE, Seq)              \
    template<>                                              \
    struct MetaDataUtil<CLASS>                              \
    {                                                       \
        MetaDataUtilIsSameImpl(CLASS, TYPE, Seq)            \
        MetaDataUtilBuildCheckImpl(CLASS, TYPE, Seq)        \
        MetaDataUtilCopyImpl(CLASS, TYPE, Seq)              \
        MetaDataUtilApplyImpl(CLASS, TYPE, Seq)             \
        MetaDataUtilFreeImpl(CLASS, TYPE, Seq)              \
    };


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
    ATTENTION: MUST BE CALLED OUTSIDE ANY NAMESPACE!!!
    ************************************    */
#define RegisterMetaType(Name, Type, Seq)               \
namespace decision_tree { namespace details {           \
    struct Name ## MetaData {                           \
        MetaDataBodyImpl(Type, Seq);                    \
    };                                                  \
    MetaDataSpecificationImpl(Name ## MetaData, Seq)    \
    MetaDataUtilSpecImpl(Name ## MetaData, TYPE, Seq)   \
}}

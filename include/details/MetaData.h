#pragma once
#include "Utils.h"
#include "Compare.h"
#include <tuple>
#include <functional>
#include <type_traits>
#include <boost/hana.hpp>
#include <boost/preprocessor.hpp>

namespace decision_tree { namespace details {
    
    template<typename MetaData>
    struct ConditionCheck
    {
        using CheckT = std::decay_t<typename MetaData::CheckT>;
        using CheckerType = bool(*)(void);      // a generic function pointer type
        using AttrFunc = utils::member_attr_func_t<CheckT, void>;

        comp::Op _op{comp::Op::Invalid};
        typename MetaData::ParamType _targetType{MetaData::ParamType::VOID};
        AttrFunc _attr{nullptr};
        CheckerType _checker;
        char _data[8];                          // a generic data pointer
    };
    // class template that handles different Checks
    template<typename MetaData, typename Target>
    struct _ConditionCheck
    {
        using CheckT = std::decay_t<typename MetaData::CheckT>;
        static_assert(decision_tree::details::utils::has_type<Target, typename MetaData::AllTypes>::value, "Target type not supported");
        using Checker = bool(*)(const typename MetaData::CheckT&, std::conditional_t<utils::pass_by_value_v<Target>, Target, const Target&>);      // std::decay first?
        using Comp = bool(*)(const Target&, const Target&, comp::Op);
        using AttrFunc = utils::member_attr_func_t<CheckT, std::conditional_t<utils::pass_by_value_v<Target>, Target, const Target&>>;

        comp::Op _op{comp::Op::Invalid};
        typename MetaData::ParamType _targetType{MetaData::ParamType::VOID};
        AttrFunc _attr{nullptr};
        union {
            char _checkerData[1];
            Checker _userChecker;
            Comp _comp;
        } _checker;
        Target _data;           // the data we use in the check
    };

    template<typename MetaData>
    struct MetaDataUtil
    {
        // helper function
        static bool is_same(const ConditionCheck<MetaData>* c1, const ConditionCheck<MetaData>* c2);     // check if two checks are the same
        template<typename Target>
        static ConditionCheck<MetaData>* buildCheck(bool(* checker_)(const typename MetaData::CheckT&, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_);
        static ConditionCheck<MetaData>* copy(ConditionCheck<MetaData>* check_);
        static void freeCheck(ConditionCheck<MetaData>* c);                               // delete a ConditionCheck object
        static bool applyCheck(const typename MetaData::CheckT& t_, ConditionCheck<MetaData>* check_);   // do check
    };
}}


/*  Some Helper Macros  */
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

/*  ParamType enum definition    */
#define MetaDataTypeEnumDef(Seq)                \
    enum class ParamType {                      \
        VOID = -1,                              \
        AllElems(Seq, _, SelectElem_1, SelectElems_1)    \
    };

/*  tuple of Comparator for each registered type    */
#define MetaDataMakeCompPair(_, __, x)  boost::hana::make_pair(ParamType::SelectElem_1(_, __, x), BOOST_PP_IF(BOOST_PP_SUB(BOOST_PP_TUPLE_SIZE(x), 2), SelectElem_2(_, __, x), std::equal_to()))
#define MetaDataMakeCompPairs(_, __, x) MetaDataMakeCompPair(_, __, x),
#define MetaDataCompDef(Seq)                    \
    constexpr static auto CompPreds = boost::hana::make_tuple(AllElems(Seq, _, MetaDataMakeCompPair, MetaDataMakeCompPairs));
    
/*  constexpr function getTypeName  */
#define MetaDataGetTypeNameImpl(_, Type, x)            \
    if constexpr (std::is_same_v<Type, SelectElem_0(_, _, x)>) {    \
        return ParamType::SelectElem_1(_, _, x);    \
    }
#define MetaDataGetTypeNameDef(Seq)             \
    template<typename Type>                     \
    static constexpr ParamType getTypeName() {  \
        AllElems(Seq, Type, MetaDataGetTypeNameImpl, MetaDataGetTypeNameImpl)   \
    }
/*  MetaData class body impl    */
#define MetaDataBodyImpl(Type, Seq)                                 \
    using CheckT = Type;                                            \
    using AllTypes = std::tuple<AllElems(Seq, _, SelectElem_0, SelectElems_0)>;  \
    MetaDataTypeEnumDef(Seq)                                        \
    MetaDataCompDef(Seq)                                            \
    MetaDataGetTypeNameDef(Seq)                                     \
                                                                    \
    template<ParamType t>                                           \
    struct getType {                                                \
        using type = void;                                          \
    };

/* template struct specification  */
#define MetaDataGetTypeSpecImpl(_, CLASS, x)                        \
    template<>                                                      \
    struct CLASS::getType<CLASS::ParamType::SelectElem_1(_, _, x)> {     \
        using type = SelectElem_0(_, _, x);                             \
    };

#define MetaDataSpecificationImpl(CLASS, Seq)               \
    AllElems(Seq, CLASS, MetaDataGetTypeSpecImpl, MetaDataGetTypeSpecImpl)

/*  MetaDataUtil function implementation - mainly switch case   */
#define MetaDataTypeEnumSwitchCase(CLASS, Seq, NormalCase, DefaultCase)     \
    BOOST_PP_SEQ_FOR_EACH(NormalCase, CLASS, Seq)                           \
    DefaultCase(CLASS)

/* bool MetaDataUtil::is_same(const ConditionCheck* c1, const ConditionCheck* c2) */
#define IsSameNormalCase(_, CLASS, x)                   \
    case CLASS::ParamType::SelectElem_1(_, _, x):              \
    {                                                   \
        using type = CLASS::getType<CLASS::ParamType::SelectElem_1(_, _, x)>::type;   \
        return boost::hana::second(boost::hana::at_c<static_cast<int>(CLASS::ParamType::SelectElem_1(_, _, x))>(CLASS::CompPreds))((*((type*)c1->_data)), (*((type*)c2->_data)));      \
    }
#define IsSameDefaultCase(CLASS)
#define MetaDataUtilIsSameImpl(CLASS, TYPE, Seq)    \
    static bool is_same(const ConditionCheck<CLASS>* c1, const ConditionCheck<CLASS>* c2) {       \
        if (c1->_targetType != c2->_targetType || c1->_checker != c2->_checker || c1->_attr != c2->_attr || c1->_op != c2->_op) \
            return false;                                                       \
        switch (c1->_targetType)                                                      \
        {                                                                       \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, IsSameNormalCase, IsSameDefaultCase) \
        }                                                                       \
        return false;                                                           \
    }

/*  template<typename Target>
    ConditionCheck<MetaData>* MetaDataUtil::buildCheck(bool(* checker_)(const MetaData::CheckT&, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_)*/
#define MetaDataUtilBuildCheckImpl(CLASS, TYPE, Seq)        \
    template<typename Target>                               \
    static ConditionCheck<CLASS>* buildCheck(bool(* checker_)(const typename CLASS::CheckT&, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_)  \
    {                                                       \
        auto check = new _ConditionCheck<CLASS, std::decay_t<Target>>();  \
        check->_targetType = CLASS::getTypeName<Target>();  \
        check->_checker._userChecker = checker_;            \
        check->_data = target_;                             \
        return (ConditionCheck<CLASS>*)check;               \
    }

/*  template<typename Target>
    ConditionCheck<MetaData>* buildCompare(details::utils::member_attr_func_t<TestMetaData::CheckT, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>> attr_, const Target& target_, details::comp::Op op_)   */
#define MetaDataUtilBuildCompareImpl(CLASS, TYPE, Seq)      \
    template<typename Target>                               \
    static ConditionCheck<CLASS>* buildCompare(details::utils::member_attr_func_t<CLASS::CheckT, std::conditional_t<!utils::pass_by_value_v<Target>, const Target&, Target>> attr_, const Target& target_, details::comp::Op op_)  \
    {                                                       \
        auto check = new _ConditionCheck<CLASS, std::decay_t<Target>>();    \
        check->_targetType = CLASS::getTypeName<Target>();  \
        check->_op = op_;                                   \
        check->_data = target_;                             \
        check->_checker._comp = &comp::compare<Target>;     \
        check->_attr = attr_;                               \
        return (ConditionCheck<CLASS>*)check;               \
    }

/*  ConditionCheck* MetaDataUtil::copy(ConditionCheck* check_)    */
#define MetaDataCopyNormalCase(_, CLASS, x)                 \
    case CLASS::ParamType::SelectElem_1(_, _, x):           \
    {                                                       \
        using type = CLASS::getType<CLASS::ParamType::SelectElem_1(_, _, x)>::type;    \
        auto in_check = new _ConditionCheck<CLASS, type>(); \
        in_check->_targetType = check_->_targetType;        \
        in_check->_op = check_->_op;                        \
        in_check->_attr = (typename _ConditionCheck<CLASS, type>::AttrFunc)check_->_attr;       \
        memcpy(in_check->_checker._checkerData, &check_->_checker, sizeof(check_->_checker));   \
        in_check->_data = *((type*)check_->_data);          \
        return (ConditionCheck<CLASS>*)in_check;            \
        break;                                              \
    }
#define MetaDataCopyDefaultCase(CLASS)
#define MetaDataUtilCopyImpl(CLASS, TYPE, Seq)              \
    static ConditionCheck<CLASS>* copy(ConditionCheck<CLASS>* check_)         \
    {                                                       \
        switch (check_->_targetType)                              \
        {                                                   \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataCopyNormalCase, MetaDataCopyDefaultCase) \
        }                                                   \
        return nullptr;                                     \
    }

/*  void MetaData::freeCheck(ConditionCheck* check_)  */
#define MetaDataFreeNormalCase(_, CLASS, x)                 \
    case CLASS::ParamType::SelectElem_1(_,_,x):              \
    {                                                       \
        using type = CLASS::getType<CLASS::ParamType::SelectElem_1(_,_,x)>::type;      \
        delete (_ConditionCheck<CLASS, type>*)check_;                \
        break;                                              \
    }
#define MetaDataFreeDefaultCase(CLASS)
#define MetaDataUtilFreeImpl(CLASS, TYPE, Seq)              \
    static void freeCheck(ConditionCheck<CLASS>* check_)             \
    {                                                       \
        switch (check_->_targetType) {                            \
            MetaDataTypeEnumSwitchCase(CLASS, Seq, MetaDataFreeNormalCase, MetaDataFreeDefaultCase) \
        }                                                   \
    }

/*  bool MetaData::applyCheck(const MetaData::CheckT& t_, MetaData::ConditionCheck* check_)  */
#define MetaDataApplyNormalCase(_, CLASS, x)                \
    case CLASS::ParamType::SelectElem_1(_,_,x):             \
    {                                                       \
        using type = CLASS::getType<CLASS::ParamType::SelectElem_1(_,_,x)>::type;           \
        if (check_->_op == details::comp::Op::Invalid) {    \
            using Checker = typename _ConditionCheck<CLASS, type>::Checker;                 \
            return details::utils::proxy_call((Checker)check_->_checker, t_, *((type*)check_->_data));  \
        }                                                   \
        else {                                              \
            using Checker = typename _ConditionCheck<CLASS, type>::Comp;                    \
            auto attrfunc = (_ConditionCheck<CLASS, type>::AttrFunc)check_->_attr;          \
            return details::utils::proxy_call_with_attribute((Checker)check_->_checker, t_, attrfunc, *((type*)check_->_data), check_->_op);    \
        }                                                   \
        break;                                              \
    }
    
#define MetaDataApplyDefaultCase(CLASS)
#define MetaDataUtilApplyImpl(CLASS, TYPE, Seq)             \
    static bool applyCheck(const typename CLASS::CheckT& t_, ConditionCheck<CLASS>* check_)  \
    {                                                       \
        switch (check_->_targetType)                              \
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
        MetaDataUtilBuildCompareImpl(CLASS, TYPE, Seq)      \
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

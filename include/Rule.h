#pragma once
#include "details/MetaData.h"
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <cassert>

namespace decision_tree {

    template<typename MetaData, typename MetaDataUtil, typename DataType>
    class DecisionTree;
    
    namespace details {
        // internal use only. Same as Rule. But won't clean check objects when destroyed
        template<typename MetaData, typename DataType>
        struct _Rule
        {
            using ConditionCheck = typename details::ConditionCheck<MetaData>;
            std::vector<ConditionCheck*> _checks;
            DataType _data;
            boost::dynamic_bitset<> _posMask;           // mark checks that should be true
            boost::dynamic_bitset<> _negMask;           // mark checks that should be false
        };
    }

    template<typename MetaData, typename MetaDataUtil, typename DataType>
    struct Rule
    {
        using ConditionCheck = typename details::ConditionCheck<MetaData>;
        using CheckT = typename MetaData::CheckT;
        using CheckTArg = typename MetaData::CheckTArg;
        friend class DecisionTree<MetaData, MetaDataUtil, DataType>;

    public:
        ~Rule()
        {
            for (auto check : _checks)
            {
                MetaDataUtil::freeCheck(check);
                check = nullptr;
            }
        }

        template<typename CheckerParam, typename Target>
        ConditionCheck* addCheck(bool(*checker_)(CheckTArg, CheckerParam), const Target& target_) {
            static_assert(std::is_convertible_v<Target, std::decay_t<CheckerParam>> || std::is_same_v<Target, std::decay_t<CheckerParam>>);
            auto check = buildCheck<std::decay_t<CheckerParam>>(checker_, target_);
            _checks.push_back(check);
            return check;
        }

        template<typename AttrFunc, typename Target, typename Op>
        ConditionCheck* addCompare(AttrFunc attr_, Target target_, Op op_) {
            static_assert(std::is_member_function_pointer_v<AttrFunc>);
            static_assert(std::is_same_v<details::utils::class_for_mem_func_t<AttrFunc>, CheckT>);
            assert(attr_ != nullptr);
            if (attr_ == nullptr) {
                return nullptr;
            }
            auto check = MetaDataUtil::buildCompare(attr_, target_, op_);
            _checks.push_back(check);
            return check;
        }

        void setData(DataType data_) {
            _data = data_;
        }

    private:
        template<typename Target>
        ConditionCheck* buildCheck(bool(* checker_)(CheckTArg, typename MetaData::template ArgType<Target>), const Target& target_)
        {
            return MetaDataUtil::buildCheck(checker_, target_);
        }

        std::vector<ConditionCheck*> _checks;
        DataType _data;
    };
}
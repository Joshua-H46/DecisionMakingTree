#pragma once
#include "details/MetaData.h"
#include <boost/dynamic_bitset.hpp>
#include <vector>

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
            DataType* _data;
            boost::dynamic_bitset<> _posMask;           // mark checks that should be true
            boost::dynamic_bitset<> _negMask;           // mark checks that should be false
        };
    }

    template<typename MetaData, typename MetaDataUtil, typename DataType>
    struct Rule
    {
        using ConditionCheck = typename details::ConditionCheck<MetaData>;
        using CheckT = typename MetaData::CheckT;
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
        ConditionCheck* addCheck(bool(*checker_)(const CheckT&, CheckerParam), const Target& target_) {
            static_assert(std::is_convertible_v<Target, std::decay_t<CheckerParam>> || std::is_same_v<Target, std::decay_t<CheckerParam>>);
            auto check = buildCheck<std::decay_t<CheckerParam>>(checker_, target_);
            _checks.push_back(check);
            return check;
        }

        void setData(DataType* data_) {
            _data = data_;
        }

    private:
        template<typename Target>
        ConditionCheck* buildCheck(bool(* checker_)(const CheckT&, std::conditional_t<!details::utils::pass_by_value_v<Target>, const Target&, Target>), const Target& target_)
        {
            return MetaDataUtil::buildCheck(checker_, target_);
        }

        std::vector<ConditionCheck*> _checks;
        DataType* _data;
    };
}
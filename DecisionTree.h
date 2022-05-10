#pragma once

#include "Rule.h"
#include <vector>
#include <list>
#include <type_traits>
#include <unordered_map>

namespace decision_tree {
    template<typename MetaData, typename ReturnT>
    class DecisionTree
    {
        using Check = typename MetaData::Check;
        using CheckT = std::decay_t<typename MetaData::CheckT>;
        using RuleType = details::_Rule<MetaData, ReturnT>;

    public:
        DecisionTree() {}
        ~DecisionTree()
        {
            for (auto&& iter : _checkerMap) {
                for (auto check : iter.second) {
                    MetaData::free(check);
                }
            }
        }

        void addRule(const Rule<MetaData, ReturnT>& rule_)
        {
            RuleType rule;
            for (auto& check : rule_._checks) {
                auto& checkList = _checkerMap[check->_checker];
                auto&& iter = std::find_if(checkList.begin(), checkList.end(), [&](const auto& item) {
                    return MetaData::is_same(check, item);
                });
                if (iter == checkList.end()) {
                    auto in_check = MetaData::copy(check);
                    checkList.push_back(in_check);
                    rule._checks.push_back(in_check);
                }
                else {
                    rule._checks.push_back(*iter);
                }
            }
            rule._data = rule_._data;
            _rules.emplace_back(std::move(rule));
        }

        // prepare the decision tree, including setting the bitmasks in all rules
        bool prepare() {
            // prepare the _checks
            std::unordered_map<Check*, size_t> posMap;
            _checks.clear();
            _checks.shrink_to_fit();
            for (auto iter : _checkerMap) {
                for (auto check : iter.second) {
                    posMap.emplace(check, _checks.size());
                    _checks.push_back(check);
                }
            }

            auto size = _checks.size();
            for (auto& rule : _rules) {
                rule._posMask.resize(size, 0);
                rule._negMask.resize(size, 0);
                for (auto check : rule._checks) {
                    rule._posMask.set(posMap[check], true);
                }
            }

            return true;
        }

        void apply(const CheckT& t, std::vector<ReturnT*>& ret) {
            // iterate over all the checks and return data in the hit rules
            boost::dynamic_bitset<> mask(_checks.size());
            for (int i=0; i<_checks.size(); i++) {
                auto res = MetaData::applyCheck(t, _checks[i]);
                mask.set(i, res);
            }
            for (auto& rule : _rules) {
                if (((mask & rule._posMask) == rule._posMask) &&
                    ((~mask & rule._negMask) == rule._negMask))
                {
                    ret.push_back(rule._data);
                }
            }
        }

    private:

        /* data */
        std::vector<RuleType> _rules;
        std::vector<Check*> _checks;             // store all the checks registered
        std::unordered_map<typename Check::CheckerType, std::list<Check*>> _checkerMap;      // map each function pointer to a list of checks (with same check but different targets)
    };
}
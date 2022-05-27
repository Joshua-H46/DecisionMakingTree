#include "../include/DecisionTree.h"
// #include <DecisionTree/DecisionTree.h>
#include "TestMetaData.h"
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>


int main()
{

    int *p1 = new int(25), *p2 = new int(28);
    int *i1 = new int(3), *i2 = new int(4);
    Data d1, d2;
    d2.id = 1002;
    decision_tree::DecisionTree<decision_tree::details::TestMetaData, decision_tree::details::MetaDataUtil<decision_tree::details::TestMetaData>, Data> dt;
    decision_tree::Rule<decision_tree::details::TestMetaData, decision_tree::details::MetaDataUtil<decision_tree::details::TestMetaData>, Data> rule1;
    decision_tree::Rule<decision_tree::details::TestMetaData, decision_tree::details::MetaDataUtil<decision_tree::details::TestMetaData>, Data> rule2;
    rule1.addCheck(checkInt, 2);
    rule1.addCheck(checkInt, 3);
    rule1.addCheck(checkDouble, 2.8);
    rule1.addCheck(checkString, "as");
    rule1.addCompare(&Data::getI, 2, decision_tree::details::comp::Operator::LessEqual{});
    rule1.addCompare(&Data::getI, 2, decision_tree::details::comp::Operator::GreaterEqual{});
    rule1.addCompare(&Data::getS, std::string("ass"), decision_tree::details::comp::Operator::Equal{});
    rule1.addCompare(&Data::getD, 1.8, decision_tree::details::comp::Operator::Greater{});
    rule1.setData(&d1);
    rule2.addCheck(checkInt, 2);
    rule2.addCheck(checkDouble, 2.6);
    rule2.addCheck(checkDouble, 2.8);
    rule2.addCheck(checkString, "as");
    rule2.addCheck(checkDouble, 23);
    rule2.setData(&d2);
    dt.addRule(rule1);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.addRule(rule1);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.prepare();
    std::vector<Data*> ret;
    ret.reserve(16);
    auto start = std::chrono::high_resolution_clock::now();
    dt.apply(d1, ret);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Return " << ret.size() << " results\n";
    for (auto data : ret) {
        std::cout << data->id << std::endl;
    }
    std::cout << "Eclipse time: " << (end-start).count() << "ns\n";
    delete p1, p2;
}
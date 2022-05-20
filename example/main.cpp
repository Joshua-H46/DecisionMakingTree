// #include "../include/DecisionTree.h"
#include <DecisionTree/DecisionTree.h>
#include "TestMetaData.h"
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

static bool checkInt(const int& q1, int q2)
{
    return q1 <= q2;
}

static bool checkDouble(const int& p1, double p2)
{
    return p1 < p2;
}

static bool checkString(const int& p1, const std::string& s)
{
    return s.size() <= p1;
}

static bool checkEnum(const int& p1, Tint t)
{
    return p1 <= (int)t;
}

static bool checkIntP(const int& p1, int* p2)
{
    if (p2 == nullptr)
        return false;
    return p1 <= *p2;
}

int main()
{

    int *p1 = new int(25), *p2 = new int(28);
    int *i1 = new int(3), *i2 = new int(4);
    int a = 2, &ra = a;
    decision_tree::DecisionTree<decision_tree::details::TestMetaData, decision_tree::details::MetaDataUtil<decision_tree::details::TestMetaData>, int> dt;
    decision_tree::Rule<decision_tree::details::TestMetaData, decision_tree::details::MetaDataUtil<decision_tree::details::TestMetaData>, int> rule1;
    decision_tree::Rule<decision_tree::details::TestMetaData, decision_tree::details::MetaDataUtil<decision_tree::details::TestMetaData>, int> rule2;
    rule1.addCheck(checkInt, a);
    rule1.addCheck(checkInt, 3);
    rule1.addCheck(checkDouble, 2.5);
    rule1.addCheck(checkString, "as");
    rule1.addCheck(checkEnum, Tint::INT);
    rule1.setData(p1);
    rule2.addCheck(checkInt, 2);
    rule2.addCheck(checkDouble, 2.2);
    rule2.addCheck(checkDouble, 2.8);
    rule2.addCheck(checkString, "as");
    rule2.addCheck(checkDouble, 23);
    rule2.addCheck(checkIntP, i1);
    rule2.addCheck(checkIntP, i2);
    // rule2.addCheck(checkString, "ass1");
    // rule2.addCheck(checkDouble, 3);
    // rule2.addCheck(checkString, "ass4");
    rule2.setData(p2);
    dt.addRule(rule1);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.addRule(rule1);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.prepare();
    std::vector<int*> ret;
    ret.reserve(16);
    auto start = std::chrono::high_resolution_clock::now();
    // for (int j=0; j<100; j++)
    // {
    //     dt.apply(j, ret);
    //     ret.clear();
    // }
    dt.apply(2, ret);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Return " << ret.size() << " results\n";
    for (auto data : ret) {
        std::cout << *data << std::endl;
    }
    std::cout << "Eclipse time: " << (end-start).count() << "ns\n";
    delete p1, p2;
}
#include "DecisionTree.h"
#include "details/MetaData.h"
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

static bool checkQty(const int& q1, int q2)
{
    return q1 <= q2;
}

static bool checkPrice(const int& p1, double p2)
{
    return p1 < p2;
}

static bool checkString(const int& p1, const std::string& s)
{
    return s.size() <= p1;
}

static bool checkLong(const int& p1, long)
{
    return true;
}


int main()
{

    int *p1 = new int(25), *p2 = new int(28);
    int a = 2, &ra = a;
    decision_tree::DecisionTree<decision_tree::details::TestMetaData, int> dt;
    decision_tree::Rule<decision_tree::details::TestMetaData, int> rule1;
    decision_tree::Rule<decision_tree::details::TestMetaData, int> rule2;
    rule1.addCheck(checkQty, a);
    rule1.addCheck(checkQty, 3);
    rule1.addCheck(checkPrice, 2.5);
    rule1.addCheck(checkString, "as");
    rule1.setData(p1);
    rule2.addCheck(checkQty, 2);
    rule2.addCheck(checkPrice, 2.2);
    rule2.addCheck(checkPrice, 2.8);
    rule2.addCheck(checkString, "ass");
    rule2.addCheck(checkPrice, 23);
    rule2.addCheck(checkString, "ass1");
    rule2.addCheck(checkPrice, 3);
    rule2.addCheck(checkString, "ass4");
    rule2.setData(p2);
    dt.addRule(rule1);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.addRule(rule1);
    dt.addRule(rule2);
    dt.addRule(rule2);
    dt.prepare();
    auto start = std::chrono::high_resolution_clock::now();
    auto ret = dt.apply(2);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Return " << ret.size() << " results\n";
    for (auto data : ret) {
        std::cout << *data << std::endl;
    }
    std::cout << "Eclipse time: " << (end-start).count() << "ns\n";
    delete p1, p2;
}
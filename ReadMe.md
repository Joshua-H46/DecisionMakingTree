# Decision Making

A header-only lib that provides high performance and flexible decision making mechanism.

## Install

sudo ./install

This will copy the header files into `/usr/include/DecisionTree/`

Example:

```bash
g++ ./example/main.cpp -std=c++17
```

## Basic idea

In short, given an object with some attributes, this lib can find all the matching ***rules*** (which may have ***checks*** on several of the attributes) you set.

Let's explain a little bit more with an example. Suppose we need to assign tasks to some students, and the rules we follow are as below:

| Class | Grade | Major | Age | Task | 
|-------|-------|-------|-----|------|
| 1     |  1    |       |     | Task1 |
| | 1 | Science | | Task2 |
| | 2 | Science | | Task3 |
|   |   | CS |  | Task4 |
|  | 2 |  | > 20 | Task5 |

The empty column means to match any. And we know that some students may match different rows and thus be assigned more than 1 tasks.

The simplest way to do this is to check row by row, column by column for each student. But this would be inefficient if the number of rule grows and common column values occur. The time complexity is O(Col*Row), or proportional to the number of cells with value.

The main idea here is: can we find all the matching rules in just one go? If we have duplicated column valus in different rows, then we should check this value only once.

## How to use

Three key parts are needed for decision making:

1. ***MetaData***: containing all the type information and helper functions, defining class `ConditionCheck`. This would be created automatically when we call macro ***RegisterMetaType***
2. ***Rule***: containing several `checks` and one `data` which is passed in by users and will be returned when the rule is matched
3. ***DecisionTree***: finding the matched rules according to the input objects, returning the data stored in rules

To use this, first we need to register types we need in the check using ***RegisterMetaTypes(Name, CheckType, Seq)***, where ***Name*** is used to define the class name, ***CheckType*** is the type of objects that we apply checks to, ***Seq*** is a sequence of tuple `(type, typename, comparator)`. Only types registered in ***Seq*** is allowed as the parameters in the `ConditionCheck`.

```c++
RegisterMetaType(Test, int,
    ((char,         CHAR))
    ((int,          INT))
    ((double,       DOUBLE))
    ((std::string,  STRING,         [](const std::string& s1, const std::string& s2) { return s1 == s2; }))
);
```
Here we define a class `TestMetaData`. The type we want to check is `int`, and four types (`char`, `int`, `double`, `std::string`) are allowed. Each type has a corresponding enum value (`ParamType::CHAR`, `ParamType::INT`, `ParamType::DOUBLE`, `ParamType::STRING`). And for `std::string` we give a self defined equal comparator

Then we should define `Rule<MetaData, ReturnT>` objects we need, where `MetaData` is the class defined above and `ReturnT` is the type we want for matching results

```c++
decision_tree::Rule<TestMetaData, int> rule1;
rule1.addCheck(checkInt, 3);
rule1.addCheck(checkDouble, 2.5);
rule1.addCheck(checkString, "ass");
int *p1 = new int(25)
rule1.setData(p1);
```

Here we use `Rule::addCheck` to attach `Checks` to `Rule` object. A `ConditionCheck` object contains two key members: a pointer to a check function and a corresponding argument which would be passed in the function when applying check. As we can see, `addCheck` is made a function template to handle different types of arguments. 
And we can set the returned data using `Rule::setData` (a pointer is passed in here so we must make sure it's safe to use later)

Then we define a `DecisionTree` object and add the rule to it. Note that the `DecisionTree` and `Rule` should share the same template arguments

```c++
decision_tree::DecisionTree<TestMetaData, int> dt;
dt.addRule(rule1);
dt.prepare();
```

`addRule` will eliminate duplicated `Checks` (with same check function and parameter) and reuse the `ConditionCheck` object. After adding all rules, `prepare` is needed to make the `DecisionTree` ready.

Finally we can apply checks to objects. The return value of `apply` is a vector of data pointers fetched from the matched rules 

```c++
std::vector<ReturnType> ret;
dt.apply(2, ret);
```

For more information, see `TestMetaData.h` and `main.cpp`

## Limitations

1. `T` and `T*` are considered different types in `RegisterMetaType`
2. Only function pointer is supported in `ConditionCheck`, which may not be flexible and safe
3. Should check if the data is `nullptr` in `Rule`

//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_VALUE_H
#define UNTITLED1_VALUE_H

enum class ValueType {
    INTEGER
};

class Integer {
public:
    Integer(int value);

    Integer();

    int value;
};

class Value {
public:
    Integer integer;
    ValueType type;

    Value(Integer integer);
};

bool equal(Value* a, Value* b);


#endif //UNTITLED1_VALUE_H

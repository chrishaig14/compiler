//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

class Package;

class Module;

class Class;

class ObjectValue;

class FunctionValue;

class ConstFunction;


enum class U_TYPE {
    PACKAGE, MODULE
};

struct Unit {
    U_TYPE type;
    union {
        Package* package;
        Module* module;
    };
};

enum class F_TYPE {
    CONST_FUNCTION, CLASS, PACKAGE, MODULE
};

struct Flirpin {
    F_TYPE type;
    union {
        ConstFunction* const_function;
        Class* clazz;
        Package* package;
        Module* module;
    };
};

enum class V_TYPE {
    OBJECT, FUNCTION
};
struct Value {
    V_TYPE type;
    union {
        FunctionValue* function_value;
        ObjectValue* object_value;
    };
};

enum class E_TYPE {
    PACKAGE, MODULE, CLASS, FUNCTION_VALUE, OBJECT_VALUE, CONST_FUNCTION
};

struct Entity {
    E_TYPE type;
    union {
        Package* package;
        Module* module;
        Class* clazz;
        FunctionValue* function_value;
        ObjectValue* object_value;
        ConstFunction* const_function;
    };
};


#endif //XLANG_ENTITY_H

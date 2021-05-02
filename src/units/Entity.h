//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

#include <string>

class Package;

class Module;

class Class;

class ObjectValue;

class FunctionValue;

class ConstFunction;

class Enum;

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
    CONST_FUNCTION, CLASS, PACKAGE, MODULE, ENUM
};

struct Flirpin {
    F_TYPE type;
    union {
        ConstFunction* const_function;
        Class* clazz;
        Package* package;
        Module* module;
        Enum* enumm;
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
    PACKAGE, MODULE, CLASS, FUNCTION_VALUE, OBJECT_VALUE, CONST_FUNCTION, ERROR, NOT_FOUND, ENUM
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
        Enum* enumm;
    };
};

std::string flirpintype_to_str(F_TYPE flirpintype);

#endif //XLANG_ENTITY_H

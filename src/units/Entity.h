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
//
// struct Entity {
//     E_TYPE type;
//     union {
//         Package* package;
//         Module* module;
//         Class* clazz;
//         FunctionValue* function_value;
//         ObjectValue* object_value;
//         ConstFunction* const_function;
//         Enum* enumm;
//     };
// };

class Entity {
public:
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

    Entity() : type(E_TYPE::NOT_FOUND), package(nullptr) {
    }

    Entity(E_TYPE type) : type(type), package(nullptr) {
    }

    Entity(Package* package) : type(E_TYPE::PACKAGE), package(package) {
    }

    Entity(Module* module) : type(E_TYPE::MODULE), module(module) {
    }

    Entity(Class* clazz) : type(E_TYPE::CLASS), clazz(clazz) {
    }

    Entity(FunctionValue* function_value) : type(E_TYPE::FUNCTION_VALUE), function_value(function_value) {
    }

    Entity(ObjectValue* object_value) : type(E_TYPE::OBJECT_VALUE), object_value(object_value) {
    }

    Entity(ConstFunction* const_function) : type(E_TYPE::CONST_FUNCTION), const_function(const_function) {
    }

    Entity(Enum* enumm) : type(E_TYPE::ENUM), enumm(enumm) {
    }
};

std::string flirpintype_to_str(F_TYPE flirpintype);

#endif //XLANG_ENTITY_H

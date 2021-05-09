//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

#include <string>

class Package;

class Module;

class Class;

class Value;

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

enum class E_TYPE {
    PACKAGE, MODULE, CLASS, VALUE, CONST_FUNCTION, ERROR, NOT_FOUND, ENUM, NOTHING
};

class Entity {
public:
    E_TYPE type;
    union {
        Package* package;
        Module* module;
        Class* clazz;
        Value* value;
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

    Entity(Value* object_value) : type(E_TYPE::VALUE), value(object_value) {
    }

    Entity(ConstFunction* const_function) : type(E_TYPE::CONST_FUNCTION), const_function(const_function) {
    }

    Entity(Enum* enumm) : type(E_TYPE::ENUM), enumm(enumm) {
    }
};

std::string flirpintype_to_str(F_TYPE flirpintype);

#endif //XLANG_ENTITY_H

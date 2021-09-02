//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

#include <string>
#include <map>
#include <memory>

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
std::ostream& operator<<(std::ostream& o, F_TYPE f);

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
protected:
    explicit Entity(E_TYPE type) : type(type) {
    }

public:
    virtual ~Entity() = default;
    const E_TYPE type;
    virtual bool equal(const Entity& other) const = 0;

    bool operator==(const Entity& rhs) const {

        bool tp = type == rhs.type;
        return tp;
    }

    bool operator!=(const Entity& rhs) const {
        return !(rhs == *this);
    }
};

class EntityClass : public Entity {
public:
    explicit EntityClass(Class* clazz) : Entity(E_TYPE::CLASS), clazz(clazz) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Class* clazz;
};

class EntityPackage : public Entity {
public:
    explicit EntityPackage(Package* package) : Entity(E_TYPE::PACKAGE), package(package) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Package* package;
};

class EntityNothing : public Entity {
public:
    explicit EntityNothing() : Entity(E_TYPE::NOTHING) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }
};

class EntityModule : public Entity {
public:
    explicit EntityModule(Module* module) : Entity(E_TYPE::MODULE), module(module) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Module* module;
};


class EntityConstFunction : public Entity {
public:
    explicit EntityConstFunction(ConstFunction* const_function)
            : Entity(E_TYPE::CONST_FUNCTION), const_function(const_function) {
    }

    ConstFunction* const_function;

    bool equal(const Entity& other) const override {
        return false;
    }
};

class EntityEnum : public Entity {
public:
    explicit EntityEnum(Enum* enumm) : Entity(E_TYPE::ENUM), enumm(enumm) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Enum* enumm;
};

class EntityNotFound : public Entity {
public:
    explicit EntityNotFound() : Entity(E_TYPE::NOT_FOUND) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }
};

class EntityError : public Entity {
public:
    explicit EntityError() : Entity(E_TYPE::ERROR) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }
};


std::string flirpintype_to_str(F_TYPE flirpintype);

#endif //XLANG_ENTITY_H

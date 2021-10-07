//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

#include <string>
#include <map>
#include <memory>
#include "../modulemember/ModuleMember.h"

class Package;

class Module;

class ConcreteClass;

class EntityValue;

class ConstFunction;

class Enum;

enum class E_TYPE {
    PACKAGE, MODULE, CLASS, VALUE, CONST_FUNCTION, ERROR, NOT_FOUND, ENUM, NOTHING
};

class EntityClass;

class EntityValue;

class EntityModule;

class EntityPackage;

class EntityValue;

class EntityNothing;

class EntityNotFound;

class EntityError;

class EntityConstFunction;

class EntityEnum;

class Entity {
protected:
    explicit Entity(E_TYPE type) : e_type(type) {
    }

public:
    virtual std::unique_ptr<Entity> clone() const = 0;
    virtual ~Entity() = default;
    const E_TYPE e_type;
    virtual bool equal(const Entity& other) const = 0;

    bool operator==(const Entity& rhs) const {

        bool tp = e_type == rhs.e_type;
        return tp;
    }

    bool operator!=(const Entity& rhs) const {
        return !(rhs == *this);
    }

    bool is_enum() const {
        return this->e_type == E_TYPE::ENUM;
    }

    bool is_class() const {
        return this->e_type == E_TYPE::CLASS;
    }

    bool is_package() const {
        return this->e_type == E_TYPE::PACKAGE;
    }

    bool is_module() const {
        return this->e_type == E_TYPE::MODULE;
    }

    bool is_nothing() const {
        return this->e_type == E_TYPE::NOTHING;
    }

    bool is_constfun() const {
        return this->e_type == E_TYPE::CONST_FUNCTION;
    }

    bool is_notfound() const {
        return this->e_type == E_TYPE::NOT_FOUND;
    }

    bool is_error() const {
        return this->e_type == E_TYPE::ERROR;

    }

    bool is_value() const {
        return this->e_type == E_TYPE::VALUE;
    }

    virtual EntityClass& get_class() {
        throw std::runtime_error("Not an EntityClass");
    }

    virtual EntityPackage& get_package() {
        throw std::runtime_error("Not an EntityPackage");
    }

    virtual EntityNothing& get_nothing() {
        throw std::runtime_error("Not an EntityNothing");
    }

    virtual EntityModule& get_module() {
        throw std::runtime_error("Not an EntityModule");
    }

    virtual EntityConstFunction& get_constfun() {
        throw std::runtime_error("Not an EntityConstFunction");
    }

    virtual EntityNotFound& get_notfound() {
        throw std::runtime_error("Not an EntityNotFound");
    }

    virtual EntityError& get_error() {
        throw std::runtime_error("Not an EntityError");
    }

    virtual EntityEnum& get_enum() {
        throw std::runtime_error("Not an EntityEnum");
    }

    virtual EntityValue& get_value() {
        throw std::runtime_error("Not an EntityValue");
    }


    virtual const EntityClass& get_class() const {
        throw std::runtime_error("Not an EntityClass");
    }

    virtual const EntityPackage& get_package() const {
        throw std::runtime_error("Not an EntityPackage");
    }

    virtual const EntityNothing& get_nothing() const {
        throw std::runtime_error("Not an EntityNothing");
    }

    virtual const EntityModule& get_module() const {
        throw std::runtime_error("Not an EntityModule");
    }

    virtual const EntityConstFunction& get_constfun() const {
        throw std::runtime_error("Not an EntityConstFunction");
    }

    virtual const EntityNotFound& get_notfound() const {
        throw std::runtime_error("Not an EntityNotFound");
    }

    virtual const EntityError& get_error() const {
        throw std::runtime_error("Not an EntityError");
    }


    virtual const EntityEnum& get_enum() const {
        throw std::runtime_error("Not an EntityEnum");
    }

    virtual const EntityValue& get_value() const {
        throw std::runtime_error("Not an EntityValue");
    }

};

std::string module_member_type_to_str(ModuleMemberType member_type);

#endif //XLANG_ENTITY_H

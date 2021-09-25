//
// Created by chris on 6/4/21.
//

#ifndef XLANG_ENTITY_H
#define XLANG_ENTITY_H

#include <string>
#include <map>
#include <memory>
#include "modulemember/ModuleMember.h"

class Package;

class Module;

class Class;

class EntityValue;

class ConstFunction;

class Enum;

enum class PackageUnitType {
    PACKAGE, MODULE
};

class Unit {
public:
    virtual bool is_module() {
        return false;
    }

    virtual bool is_package() {
        return false;
    }

    virtual Package& package() {
        throw std::runtime_error("Unit is not a package");
    }

    virtual Module& module() {
        throw std::runtime_error("Unit is not a module");
    }
};

class SubpackageUnit : public Unit {
    Package* _package;
public:

    explicit SubpackageUnit(Package* _package) : _package(_package) {
    }

    Package& package() override {
        return *this->_package;
    }

    bool is_package() override {
        return true;
    }
};

class ModuleUnit : public Unit {
    Module* _module;
public:

    explicit ModuleUnit(Module* _module) : _module(_module) {
    }

    Module& module() override {
        return *this->_module;
    }

    bool is_module() override {
        return true;
    }
};

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
    virtual Entity* clone() const = 0;
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

    virtual bool is_class() const {
        return false;
    }

    virtual bool is_package() const {
        return false;
    }

    virtual bool is_module() const {
        return false;
    }

    virtual bool is_nothing() const {
        return false;
    }

    virtual bool is_constfun() const {
        return false;
    }

    virtual bool is_error() const {
        return false;
    }

    virtual bool is_value() const {
        return false;
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

class EntityClass : public Entity {
public:
    explicit EntityClass(Class& clazz) : Entity(E_TYPE::CLASS), clazz(clazz) {
    }

    bool equal(const Entity& other) const override {

        return &this->clazz == &((const EntityClass&) other).clazz;
    }

    Entity* clone() const override {
        return new EntityClass(this->clazz);
    }

    Class& clazz;

    EntityClass& get_class() override {
        return *this;
    }

    const EntityClass& get_class() const override {
        return *this;
    }
};

class EntityPackage : public Entity {
public:
    explicit EntityPackage(Package& package) : Entity(E_TYPE::PACKAGE), package(package) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityPackage(this->package);
    }

    Package& package;

    EntityPackage& get_package() override {
        return *this;
    }

    const EntityPackage& get_package() const override {
        return *this;
    }
};

class EntityNothing : public Entity {
public:
    explicit EntityNothing() : Entity(E_TYPE::NOTHING) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }


    Entity* clone() const override {
        return new EntityNothing();
    }

    EntityNothing& get_nothing() override {
        return *this;
    }

    const EntityNothing& get_nothing() const override {
        return *this;
    }
};

class EntityModule : public Entity {
public:
    explicit EntityModule(Module& module) : Entity(E_TYPE::MODULE), module(module) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityModule(this->module);
    }

    Module& module;

    EntityModule& get_module() override {
        return *this;
    }

    const EntityModule& get_module() const override {
        return *this;
    }
};


class EntityConstFunction : public Entity {
public:
    explicit EntityConstFunction(ConstFunction& const_function)
            : Entity(E_TYPE::CONST_FUNCTION), const_function(const_function) {
    }

    ConstFunction& const_function;

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityConstFunction(this->const_function);
    }

    EntityConstFunction& get_constfun() override {
        return *this;
    }

    const EntityConstFunction& get_constfun() const override {
        return *this;
    }
};

class EntityEnum : public Entity {
public:
    explicit EntityEnum(Enum& enumm) : Entity(E_TYPE::ENUM), enumm(enumm) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityEnum(this->enumm);
    }

    EntityEnum& get_enum() override {
        return *this;
    }

    const EntityEnum& get_enum() const override {
        return *this;
    }

    Enum& enumm;
};

class EntityNotFound : public Entity {
public:
    explicit EntityNotFound() : Entity(E_TYPE::NOT_FOUND) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    EntityNotFound* clone() const override {
        return new EntityNotFound();
    }

    EntityNotFound& get_notfound() override {
        return *this;
    }

    const EntityNotFound& get_notfound() const override {
        return *this;
    }
};

class EntityError : public Entity {
public:
    explicit EntityError() : Entity(E_TYPE::ERROR) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    EntityError* clone() const override {
        return new EntityError();
    }

    EntityError& get_error() override {
        return *this;
    }

    const EntityError& get_error() const override {
        return *this;
    }
};


std::string module_member_type_to_str(ModuleMemberType member_type);

#endif //XLANG_ENTITY_H

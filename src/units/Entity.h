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

enum class ModuleMemberType {
    CONST_FUNCTION, CLASS, PACKAGE, MODULE, ENUM
};
std::ostream& operator<<(std::ostream& o, ModuleMemberType f);

class ModuleMember {
public:
    virtual bool is_klass() {
        return false;
    }

    virtual Class& klass() {
        throw std::runtime_error("ModuleMember is not a Class");
    }

    virtual bool is_const_function() {
        return false;
    }

    virtual ConstFunction& const_function() {
        throw std::runtime_error("ModuleMember is not a ConstFunction");
    }

    virtual bool is_module() {
        return false;
    }

    virtual Module& module() {
        throw std::runtime_error("ModuleMember is not a Module");
    }

    virtual bool is_package() {
        return false;
    }

    virtual Package& package() {
        throw std::runtime_error("ModuleMember is not a Package");
    }

    virtual bool is_enumm() {
        return false;
    }

    virtual Enum& enumm() {
        throw std::runtime_error("ModuleMember is not a Enum");
    }
};

class ClassModuleMember : public ModuleMember {
    Class* _klass;
public:
    ClassModuleMember(Class* _klass) : _klass(_klass) {
    }

    bool is_klass() override {
        return true;
    }

    Class& klass() override {
        return *this->_klass;
    }
};

class ConstFunctionModuleMember : public ModuleMember {
    ConstFunction* _const_function;
public:
    ConstFunctionModuleMember(ConstFunction* _const_function) : _const_function(_const_function) {
    }

    bool is_const_function() override {
        return true;
    }

    ConstFunction& const_function() override {
        return *this->_const_function;
    }
};

class ModuleModuleMember : public ModuleMember {
    Module* _module;
public:
    ModuleModuleMember(Module* _module) : _module(_module) {
    }

    bool is_module() override {
        return true;
    }

    Module& module() override {
        return *this->_module;
    }
};

class EnumModuleMember : public ModuleMember {
    Enum* _enumm;
public:
    EnumModuleMember(Enum* _enumm) : _enumm(_enumm) {
    }

    bool is_enumm() override {
        return true;
    }

    Enum& enumm() override {
        return *this->_enumm;
    }
};


class PackageModuleMember : public ModuleMember {
    Package* _package;
public:
    PackageModuleMember(Package* _package) : _package(_package) {
    }

    bool is_package() override {
        return true;
    }

    Package& package() override {
        return *this->_package;
    }
};


enum class E_TYPE {
    PACKAGE, MODULE, CLASS, VALUE, CONST_FUNCTION, ERROR, NOT_FOUND, ENUM, NOTHING
};

class Entity {
protected:
    explicit Entity(E_TYPE type) : type(type) {
    }

public:
    virtual Entity* clone() const = 0;
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

        return this->clazz == ((const EntityClass&) other).clazz;
    }

    Entity* clone() const override {
        return new EntityClass(this->clazz);
    }

    Class* clazz;
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
};

class EntityEnum : public Entity {
public:
    explicit EntityEnum(Enum* enumm) : Entity(E_TYPE::ENUM), enumm(enumm) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityEnum(this->enumm);
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

    EntityNotFound* clone() const override {
        return new EntityNotFound();
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
};


std::string module_member_type_to_str(ModuleMemberType member_type);

#endif //XLANG_ENTITY_H

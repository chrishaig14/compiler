//
// Created by chris on 25/9/21.
//

#ifndef XLANG_MODULEMEMBER_H
#define XLANG_MODULEMEMBER_H

#include <ostream>

enum class ModuleMemberType {
    CONST_FUNCTION, CLASS, PACKAGE, MODULE, ENUM
};
std::ostream& operator<<(std::ostream& o, ModuleMemberType f);

class Class;

class Enum;

class ConstFunction;

class Module;

class Package;

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

#endif //XLANG_MODULEMEMBER_H

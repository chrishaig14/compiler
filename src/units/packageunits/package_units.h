//
// Created by chris on 25/9/21.
//

#ifndef XLANG_PACKAGE_UNITS_H
#define XLANG_PACKAGE_UNITS_H

#include <stdexcept>

class Package;

class Module;

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

    virtual ~Unit() = default;
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

#endif //XLANG_PACKAGE_UNITS_H

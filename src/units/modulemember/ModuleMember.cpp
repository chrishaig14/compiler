//
// Created by chris on 25/9/21.
//

#include <memory>
#include "ModuleMember.h"


bool ModuleMember::is_klass() {
    return false;
}

Class& ModuleMember::klass() {
    throw std::runtime_error("ModuleMember is not a Class");
}

bool ModuleMember::is_const_function() {
    return false;
}

ConstFunction& ModuleMember::const_function() {
    throw std::runtime_error("ModuleMember is not a ConstFunction");
}

bool ModuleMember::is_module() {
    return false;
}

Module& ModuleMember::module() {
    throw std::runtime_error("ModuleMember is not a Module");
}

bool ModuleMember::is_package() {
    return false;
}

Package& ModuleMember::package() {
    throw std::runtime_error("ModuleMember is not a Package");
}

bool ModuleMember::is_enumm() {
    return false;
}

Enum& ModuleMember::enumm() {
    throw std::runtime_error("ModuleMember is not a Enum");
}

ClassModuleMember::ClassModuleMember(Class* _klass) : p_klass(_klass) {
}

bool ClassModuleMember::is_klass() {
    return true;
}

Class& ClassModuleMember::klass() {
    return *this->p_klass;
}

std::unique_ptr<ModuleMember> ClassModuleMember::clone() {
    return std::make_unique<ClassModuleMember>(this->p_klass);
}

ConstFunctionModuleMember::ConstFunctionModuleMember(ConstFunction* _const_function)
        : p_const_function(_const_function) {
}

bool ConstFunctionModuleMember::is_const_function() {
    return true;
}

ConstFunction& ConstFunctionModuleMember::const_function() {
    return *this->p_const_function;
}

std::unique_ptr<ModuleMember> ConstFunctionModuleMember::clone() {
    return std::make_unique<ConstFunctionModuleMember>(this->p_const_function);
}

ModuleModuleMember::ModuleModuleMember(Module* _module) : p_module(_module) {
}

bool ModuleModuleMember::is_module() {
    return true;
}

Module& ModuleModuleMember::module() {
    return *this->p_module;
}

std::unique_ptr<ModuleMember> ModuleModuleMember::clone() {
    return std::make_unique<ModuleModuleMember>(this->p_module);
}

EnumModuleMember::EnumModuleMember(Enum* _enumm) : p_enum(_enumm) {
}

bool EnumModuleMember::is_enumm() {
    return true;
}

Enum& EnumModuleMember::enumm() {
    return *this->p_enum;
}

std::unique_ptr<ModuleMember> EnumModuleMember::clone() {
    return std::make_unique<EnumModuleMember>(this->p_enum);
}

PackageModuleMember::PackageModuleMember(Package* _package) : p_package(_package) {
}

bool PackageModuleMember::is_package() {
    return true;
}

Package& PackageModuleMember::package() {
    return *this->p_package;
}

std::unique_ptr<ModuleMember> PackageModuleMember::clone() {
    return std::make_unique<PackageModuleMember>(this->p_package);
}

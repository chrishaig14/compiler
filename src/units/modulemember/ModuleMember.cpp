//
// Created by chris on 25/9/21.
//

#include <memory>
#include "ModuleMember.h"


bool ModuleMember::is_klass() const {
    return false;
}

ConcreteClass& ModuleMember::klass() const {
    throw std::runtime_error("ModuleMember is not a Class");
}

bool ModuleMember::is_const_function() const {
    return false;
}

const ConstFunction& ModuleMember::const_function() const {
    throw std::runtime_error("ModuleMember is not a ConstFunction");
}

bool ModuleMember::is_module() const {
    return false;
}

Module& ModuleMember::module() const {
    throw std::runtime_error("ModuleMember is not a Module");
}

bool ModuleMember::is_package() const {
    return false;
}

Package& ModuleMember::package() const {
    throw std::runtime_error("ModuleMember is not a Package");
}

bool ModuleMember::is_enumm() const {
    return false;
}

Enum& ModuleMember::enumm() const {
    throw std::runtime_error("ModuleMember is not a Enum");
}

bool ModuleMember::is_typeclass() const {
    return false;
}

TypeclassFoo& ModuleMember::typeclass() const {
    throw std::runtime_error("ModuleMember is not a Typeclass");
}

bool ModuleMember::is_template_klass() const {
    return false;
}

TemplateClassInfo& ModuleMember::template_klass() const {
    throw std::runtime_error("ModuleMember is not a template_klass");
}

ClassModuleMember::ClassModuleMember(ConcreteClass& _klass) : p_klass(_klass) {
}

bool ClassModuleMember::is_klass() const {
    return true;
}

ConcreteClass& ClassModuleMember::klass() const {
    return this->p_klass;
}

std::unique_ptr<ModuleMember> ClassModuleMember::clone() {
    return std::make_unique<ClassModuleMember>(this->p_klass);
}

TemplateClassModuleMember::TemplateClassModuleMember(TemplateClassInfo& _klass) : p_klass(_klass) {
}

bool TemplateClassModuleMember::is_template_klass() const {
    return true;
}

TemplateClassInfo& TemplateClassModuleMember::template_klass() const {
    return this->p_klass;
}

std::unique_ptr<ModuleMember> TemplateClassModuleMember::clone() {
    return std::make_unique<TemplateClassModuleMember>(this->p_klass);
}

ConstFunctionModuleMember::ConstFunctionModuleMember(const ConstFunction& _const_function) : p_const_function(
        _const_function) {
}

bool ConstFunctionModuleMember::is_const_function() const {
    return true;
}

const ConstFunction& ConstFunctionModuleMember::const_function() const {
    return this->p_const_function;
}

std::unique_ptr<ModuleMember> ConstFunctionModuleMember::clone() {
    return std::make_unique<ConstFunctionModuleMember>(this->p_const_function);
}

ModuleModuleMember::ModuleModuleMember(Module* _module) : p_module(_module) {
}

bool ModuleModuleMember::is_module() const {
    return true;
}

Module& ModuleModuleMember::module() const {
    return *this->p_module;
}

std::unique_ptr<ModuleMember> ModuleModuleMember::clone() {
    return std::make_unique<ModuleModuleMember>(this->p_module);
}

EnumModuleMember::EnumModuleMember(Enum* _enumm) : p_enum(_enumm) {
}

bool EnumModuleMember::is_enumm() const {
    return true;
}

Enum& EnumModuleMember::enumm() const {
    return *this->p_enum;
}

std::unique_ptr<ModuleMember> EnumModuleMember::clone() {
    return std::make_unique<EnumModuleMember>(this->p_enum);
}

PackageModuleMember::PackageModuleMember(Package* _package) : p_package(_package) {
}

bool PackageModuleMember::is_package() const {
    return true;
}

Package& PackageModuleMember::package() const {
    return *this->p_package;
}

std::unique_ptr<ModuleMember> PackageModuleMember::clone() {
    return std::make_unique<PackageModuleMember>(this->p_package);
}

TypeclassFoo& TypeclassModuleMember::typeclass() const {
    return *this->p_typeclass;
}

bool TypeclassModuleMember::is_typeclass() const {
    return true;
}

std::unique_ptr<ModuleMember> TypeclassModuleMember::clone() {
    return std::make_unique<TypeclassModuleMember>(this->p_typeclass);
}

TypeclassModuleMember::TypeclassModuleMember(TypeclassFoo* _enumm) : p_typeclass(_enumm) {
}

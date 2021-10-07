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

class ConcreteClass;

class Enum;

class ConstFunction;

class Module;

class Package;

class ModuleMember {
public:
    virtual std::unique_ptr<ModuleMember> clone() = 0;

    virtual bool is_klass();

    virtual ConcreteClass& klass();

    virtual bool is_const_function();

    virtual ConstFunction& const_function();

    virtual bool is_module();

    virtual Module& module();

    virtual bool is_package();

    virtual Package& package();

    virtual bool is_enumm();

    virtual Enum& enumm();

    virtual ~ModuleMember() = default;
};

class ClassModuleMember : public ModuleMember {
    ConcreteClass& p_klass;
public:
    explicit ClassModuleMember(ConcreteClass& _klass);


    bool is_klass() override;

    ConcreteClass& klass() override;

    std::unique_ptr<ModuleMember> clone() override;
};

class ConstFunctionModuleMember : public ModuleMember {
    ConstFunction& p_const_function;
public:
    explicit ConstFunctionModuleMember(ConstFunction& _const_function);

    bool is_const_function() override;

    ConstFunction& const_function() override;
    std::unique_ptr<ModuleMember> clone() override;
};

class ModuleModuleMember : public ModuleMember {
    Module* p_module;
public:
    explicit ModuleModuleMember(Module* _module);

    bool is_module() override;

    Module& module() override;
    std::unique_ptr<ModuleMember> clone() override;
};

class EnumModuleMember : public ModuleMember {
    Enum* p_enum;
public:
    explicit EnumModuleMember(Enum* _enumm);

    bool is_enumm() override;

    Enum& enumm() override;
    std::unique_ptr<ModuleMember> clone() override;
};

class PackageModuleMember : public ModuleMember {
    Package* p_package;
public:
    explicit PackageModuleMember(Package* _package);

    bool is_package() override;

    Package& package() override;
    std::unique_ptr<ModuleMember> clone() override;
};

#endif //XLANG_MODULEMEMBER_H

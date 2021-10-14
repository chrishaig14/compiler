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

class TypeclassFoo;

class ConstFunction;

class Module;

class Package;

class ModuleMember {
public:
    virtual std::unique_ptr<ModuleMember> clone() = 0;

    virtual bool is_klass() const;

    virtual ConcreteClass& klass() const;

    virtual bool is_const_function() const;

    virtual ConstFunction& const_function() const;

    virtual bool is_module() const;

    virtual bool is_typeclass() const;

    virtual Module& module() const;

    virtual TypeclassFoo& typeclass() const;

    virtual bool is_package() const;

    virtual Package& package() const;

    virtual bool is_enumm() const;

    virtual Enum& enumm() const;

    virtual ~ModuleMember() = default;
};

class ClassModuleMember : public ModuleMember {
    ConcreteClass& p_klass;
public:
    explicit ClassModuleMember(ConcreteClass& _klass);


    bool is_klass()  const override;

    ConcreteClass& klass()  const override;

    std::unique_ptr<ModuleMember> clone() override;
};

class ConstFunctionModuleMember : public ModuleMember {
    ConstFunction& p_const_function;
public:
    explicit ConstFunctionModuleMember(ConstFunction& _const_function);

    bool is_const_function() const  override;

    ConstFunction& const_function()  const override;
    std::unique_ptr<ModuleMember> clone() override;
};

class ModuleModuleMember : public ModuleMember {
    Module* p_module;
public:
    explicit ModuleModuleMember(Module* _module);

    bool is_module()  const override;

    Module& module()  const override;
    std::unique_ptr<ModuleMember> clone() override;
};

class EnumModuleMember : public ModuleMember {
    Enum* p_enum;
public:
    explicit EnumModuleMember(Enum* _enumm);

    bool is_enumm()  const override;

    Enum& enumm()  const override;
    std::unique_ptr<ModuleMember> clone() override;
};

class TypeclassModuleMember : public ModuleMember {
    TypeclassFoo* p_typeclass;
public:
    explicit TypeclassModuleMember(TypeclassFoo* _enumm);

    bool is_typeclass()  const override;

    TypeclassFoo& typeclass()  const override;

    std::unique_ptr<ModuleMember> clone() override;
};


class PackageModuleMember : public ModuleMember {
    Package* p_package;
public:
    explicit PackageModuleMember(Package* _package);

    bool is_package()  const override;

    Package& package()  const override;
    std::unique_ptr<ModuleMember> clone() override;
};

#endif //XLANG_MODULEMEMBER_H

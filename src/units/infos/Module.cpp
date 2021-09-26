//
// Created by chris on 6/4/21.
//

#include "Module.h"

Module::Module(Path path, std::string abs_path, bool is_lib)
        : name(path.as_vec().back()), abs_path(abs_path), is_lib(is_lib), path(path) {
}

ModuleMember* Module::get(Path p) {
    VectorOfStrings pt = p.as_vec();
    return this->members[pt.back()];
}

void Module::add_class_definition(Class* p_class) {
    this->classes.push_back(std::unique_ptr<Class>(p_class));
    this->members[p_class->class_name] = new ClassModuleMember(p_class);
}

void Module::add_enum_definition(Enum* enumm) {
    this->enums.push_back(std::unique_ptr<Enum>(enumm));
    this->members[enumm->enumm_name] = new EnumModuleMember(enumm);
}

void Module::add_func_definition(ConstFunction* const_function) {
    this->const_functions.push_back(std::unique_ptr<ConstFunction>(const_function));
    this->members[const_function->path.as_vec().back()] = new ConstFunctionModuleMember(const_function);
}

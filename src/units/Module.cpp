//
// Created by chris on 6/4/21.
//

#include "Module.h"

Module::Module(Path path, std::string abs_path, std::string rel_path, bool is_lib)
        : name(path.as_vec().back()), abs_path(abs_path), is_lib(is_lib), path(path) {
}

Flirpin Module::get(Path p) {
    VectorOfStrings pt = p.as_vec();
    return this->flirpins[pt.back()];
}

void Module::add_class_definition(Class* p_class) {
    this->classes.push_back(std::unique_ptr<Class>(p_class));
    this->flirpins[p_class->class_name] = Flirpin{.type=F_TYPE::CLASS, .clazz=p_class};
}

void Module::add_enum_definition(Enum* enumm) {
    this->enums.push_back(std::unique_ptr<Enum>(enumm));
    this->flirpins[enumm->enumm_name] = Flirpin{.type=F_TYPE::ENUM, .enumm=enumm};
}

void Module::add_func_definition(ConstFunction* const_function) {
    this->const_functions.push_back(std::unique_ptr<ConstFunction>(const_function));
    this->flirpins[const_function->path.as_vec().back()] = Flirpin{.type=F_TYPE::CONST_FUNCTION, .const_function=const_function};
}

//
// Created by chris on 6/11/21.
//

#include <semantic/ModuleChecker.h>
#include "GlobalInstancer.h"

std::map<std::string, sem::UTypeFunction>
g_instantiate_typeclass_methods(TypeclassFoo& typeclass, const ast::ObjectType& type) {
    std::map<std::string, sem::UTypeFunction> out_methods;
    for (auto& method: typeclass.methods) {
        std::unordered_map<std::string, ast::Type*> repl;
        repl[typeclass.gen_type] = type.clone();
        std::cout << "type is: " << type.to_string() << std::endl;
        std::cout << "original is: " << method.second->to_string() << std::endl;
        auto ret = make_type(*method.second->to_ast(), repl);
        std::cout << "ret is: " << ret->to_string() << std::endl;
        out_methods[method.first] = sem::UTypeFunction(static_cast<sem::TypeFunction*>(ret->to_sem()));
    }
    return out_methods;
}

void GlobalInstancer::add_instance_to_class(ConcreteClass& clazz, Path instance) {
    auto mm = this->top_package.get(instance);
    auto& typeclass = mm->typeclass();
    Path module_path = typeclass.path.pathbase();
    auto* module_p = this->top_package.get_module(module_path);
    assert(module_p != nullptr);
    auto& module = *module_p;

    // auto& mod = module->module();
    std::cout << "module path for typeclass: " << module_path.as_str() << std::endl;
    auto cls_module_path = clazz.path.pathbase();
    auto& cls_module = *this->top_package.get_module(cls_module_path);
    sem::TypeObject cls_type(clazz.class_name);
    cls_module.fill_actual(cls_type);
    auto out_methods = g_instantiate_typeclass_methods(typeclass, (const ast::ObjectType&) *cls_type.to_ast());
    for (auto& m: typeclass.methods) {
        sem::TypeFunction& ft = *m.second;
        std::unordered_map<std::string, ast::Type*> repl;
        repl[typeclass.gen_type] = cls_type.to_ast();
        auto cl = make_type(*ft.to_ast(), repl);
        auto cl_sem = cl->to_sem();
        module.fill_actual(*cl_sem);
        clazz.methods.emplace(m.first,
                              InstanceMethod(instance,
                                             BaseMethod(false,
                                                        ConstFunction(Path(typeclass.path, m.first),
                                                                      static_cast<sem::TypeFunction&>(*cl_sem)))));
        clazz.all_members[m.first] = ClassMemberCategory::method;
    }
    for (auto& m: typeclass.static_methods) {
        sem::TypeFunction& ft = *m.second;
        clazz.methods.emplace(m.first,
                              InstanceMethod(instance,
                                             BaseMethod(true, ConstFunction(Path(typeclass.path, m.first), ft))));
        clazz.all_members[m.first] = ClassMemberCategory::method;
    }
    // for (auto& m: ) {
    //     clazz.methods[m.first] = m.second;
    // }x
}

void GlobalInstancer::add_instances_to_class(ConcreteClass& clazz, std::set<std::string>& class_instances) {
    for (auto& instance: class_instances) {
        this->add_instance_to_class(clazz, Path(instance));
    }
}

void GlobalInstancer::apply() {
    for (auto& clazz: this->all_classes) {
        this->add_instances_to_class(clazz, this->instances[clazz.get().path.as_str()]);
    }
}

GlobalInstancer::GlobalInstancer(Package& top_package, std::map<std::string, std::set<std::string>>& instances,
                                 std::vector<std::reference_wrapper<ConcreteClass>>& all_classes) : top_package(
        top_package), instances(instances), all_classes(all_classes) {
}

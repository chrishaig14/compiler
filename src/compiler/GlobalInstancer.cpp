//
// Created by chris on 6/11/21.
//

#include "GlobalInstancer.h"

void GlobalInstancer::add_instance_to_class(ConcreteClass& clazz, Path instance) {
    auto mm = this->top_package.get(instance);
    auto& typeclass = mm->typeclass();
    for (auto& m: typeclass.methods) {
        clazz.methods[m.first] = std::make_unique<InstanceMethod>(instance,
                                                                  std::make_unique<BaseMethod>(false,
                                                                                               std::make_unique<ConstFunction>(
                                                                                                       Path(typeclass.path,
                                                                                                            m.first),
                                                                                                       sem::UTypeFunction(
                                                                                                               m.second->clone()))));
        clazz.all_members[m.first] = ClassMemberCategory::method;
    }
    for (auto& m: typeclass.static_methods) {
        clazz.methods[m.first] = std::make_unique<InstanceMethod>(instance,
                                                                  std::make_unique<BaseMethod>(true,
                                                                                               std::make_unique<ConstFunction>(
                                                                                                       Path(typeclass.path,
                                                                                                            m.first),
                                                                                                       sem::UTypeFunction(
                                                                                                               m.second->clone()))));
        clazz.all_members[m.first] = ClassMemberCategory::method;
    }
    // for (auto& m: ) {
    //     clazz.methods[m.first] = m.second;
    // }
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

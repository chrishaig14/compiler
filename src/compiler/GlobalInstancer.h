//
// Created by chris on 6/11/21.
//

#ifndef XLANG_GLOBALINSTANCER_H
#define XLANG_GLOBALINSTANCER_H


#include <units/infos/Package.h>
#include <units/infos/ConcreteClass.h>
#include <set>

class GlobalInstancer {
    Package& top_package;
    std::map<std::string, std::set<std::string>>& instances;
    std::vector<std::reference_wrapper<ConcreteClass>>& all_classes;
public:

    GlobalInstancer(Package& top_package, std::map<std::string, std::set<std::string>>& instances,
                    std::vector<std::reference_wrapper<ConcreteClass>>& all_classes);

    void apply();

    void add_instance_to_class(ConcreteClass& clazz, Path instance);

    void add_instances_to_class(ConcreteClass& clazz, std::set<std::string>& class_instances);
};


#endif //XLANG_GLOBALINSTANCER_H

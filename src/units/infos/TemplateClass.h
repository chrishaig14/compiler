//
// Created by chris on 2/8/20.
//

#ifndef TEMPLATECLASSINFO_H
#define TEMPLATECLASSINFO_H

#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include "../../ast/general/Type.h"
#include "../../util/macros.h"
#include "../entities/Entity.h"
#include "ConstFunction.h"
#include "ConcreteClass.h"

class TemplateClassInfo {
public:

    std::map<std::string, std::unique_ptr<ConcreteClass>> generic_instances;

    TemplateClassInfo(const std::string& class_name, Path path, VectorOfStrings type_params);

    VectorOfStrings member_names;
    std::vector<ast::Type*> member_types;
    std::unordered_map<std::string, std::unique_ptr<Entity>> member_entities;

    MapStringType members;
    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> methods;

    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members;
    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> static_methods;

    const Path path;
    const std::string class_name;

    ~TemplateClassInfo();

    bool operator!=(const TemplateClassInfo& b) const;

    bool operator==(const TemplateClassInfo& b) const;
    VectorOfStrings type_params;
};

#endif //TEMPLATECLASSINFO_H

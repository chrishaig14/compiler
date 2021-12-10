//
// Created by chris on 4/10/20.
//

#include "TemplateClassDef.h"

using namespace ast;

TemplateClassDef::TemplateClassDef(const std::string& className, VectorOfStrings type_parameters,
                                   std::vector<ast::ClassAttribute> attributes,
                                   std::vector<std::unique_ptr<AstMethod>> methods,
                                   std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_attributes,
                                   TextPosition start, TextPosition end) : ast::TopNode(TopNodeType::TEMPLATE_CLS,
                                                                                        start,
                                                                                        end),
                                                                           attributes(std::move(attributes)),
                                                                           static_attributes(static_attributes),
                                                                           methods(std::move(methods)),
                                                                           class_name(className) {
    this->type_parameters = type_parameters;
}

bool TemplateClassDef::equal(const ast::TopNode& other) const {
    return false;
}


TemplateClassDef::~TemplateClassDef() {
    // for (const auto& mem: this->members) {
    //     delete mem.second;
    // }
    // for (const auto& method: this->methods) {
    //     delete method.second;
    // }
}

nlohmann::json TemplateClassDef::to_json() const {
    nlohmann::json j;
    j["type"] = "class";
    std::vector<nlohmann::json> memj;
    for (auto& attr : this->attributes) {
        memj.push_back({{"id",   attr.id},
                        {"type", attr.type->to_json()}});
    }
    nlohmann::json methj;
    // for (auto& m: this->methods) {
    //     methj[m.first] = m.second->to_json();
    // }
    nlohmann::json smethj;
    // for (auto& meth: this->static_methods) {
    //     smethj[meth.first] = meth.second->to_json();
    // }
    j["class"] = {{"id",             this->class_name},
                  {"attributes",     memj},
                  {"methods",        methj},
                  {"static_methods", smethj}};
    return j;
}

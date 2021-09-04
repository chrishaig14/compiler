//
// Created by chris on 4/10/20.
//

#include "Klass.h"

using namespace ast;

Klass::Klass(const std::string& className, VectorOfStrings type_parameters,
             std::vector<std::pair<std::string, ast::UTypeNode>> members,
             std::unordered_map<std::string, std::unique_ptr<KMethod>> functions,
             std::map<std::string, std::pair<ast::TypeNode*, ast::Node*>> static_members,
             std::unordered_map<std::string, ast::UFunctionNode>& static_methods, TextPosition start, TextPosition end)
        : ast::Node(NodeType::CLS, start, end), members(std::move(members)), static_members(static_members),
          methods(std::move(functions)), static_methods(std::move(static_methods)), class_name(className) {
    this->type_parameters = type_parameters;
}

bool Klass::equal(const ast::Node& other) const {
    return false;
}


Klass::~Klass() {
    // for (const auto& mem: this->members) {
    //     delete mem.second;
    // }
    // for (const auto& method: this->methods) {
    //     delete method.second;
    // }
}

nlohmann::json Klass::to_json() const {
    nlohmann::json j;
    j["type"] = "class";
    std::vector<nlohmann::json> memj;
    for (auto& i : this->members) {
        memj.push_back({{"id",   i.first},
                        {"type", i.second->to_json()}});
    }
    nlohmann::json methj;
    for (auto& m: this->methods) {
        methj[m.first] = m.second->method->to_json();
    }
    nlohmann::json smethj;
    for (auto& m: this->static_methods) {
        smethj[m.first] = m.second->to_json();
    }
    j["class"] = {{"id",             this->class_name},
                  {"members",        memj},
                  {"methods",        methj},
                  {"static_methods", smethj}};
    return j;
}

KMethod::KMethod(Implicit* constraint, ast::UFunctionNode method) : constraint(constraint), method(std::move(method)) {
}

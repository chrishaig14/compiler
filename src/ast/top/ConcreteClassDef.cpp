//
// Created by chris on 4/10/20.
//

#include "ConcreteClassDef.h"

using namespace ast;

// ConcreteClassDef(const std::string& className, std::vector<std::pair<std::string, ast::UTypeNode>> members,
//                  std::unordered_map<std::string, ast::UFunctionNode> functions,
//                  std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members,
//                  std::unordered_map<std::string, ast::UFunctionNode>& static_methods, TextPosition start,
//                  TextPosition end);

ConcreteClassDef::ConcreteClassDef(const std::string& className,
                                   std::vector<std::pair<std::string, ast::UTypeNode>> members,
                                   std::unordered_map<std::string, ast::UFunctionNode> functions,
                                   std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members,
                                   std::unordered_map<std::string, ast::UFunctionNode>& static_methods,
                                   TextPosition start, TextPosition end) : ast::TopNode(TopNodeType::CONCRETE_CLS, start, end),
                                                                           members(std::move(members)),
                                                                           static_members(static_members),
                                                                           methods(std::move(functions)),
                                                                           static_methods(std::move(static_methods)),
                                                                           class_name(className) {
}

bool ConcreteClassDef::equal(const ast::TopNode& other) const {
    return false;
}


ConcreteClassDef::~ConcreteClassDef() {
    // for (const auto& mem: this->members) {
    //     delete mem.second;
    // }
    // for (const auto& method: this->methods) {
    //     delete method.second;
    // }
}

nlohmann::json ConcreteClassDef::to_json() const {
    nlohmann::json j;
    j["type"] = "class";
    std::vector<nlohmann::json> memj;
    for (auto& i : this->members) {
        memj.push_back({{"id",   i.first},
                        {"type", i.second->to_json()}});
    }
    nlohmann::json methj;
    for (auto& m: this->methods) {
        methj[m.first] = m.second->to_json();
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


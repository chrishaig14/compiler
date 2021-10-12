//
// Created by chris on 31/7/21.
//

#include "TypeclassAst.h"

using namespace ast;

nlohmann::json TypeclassAst::to_json() const {
    nlohmann::json j;
    j["id"] = this->id;
    j["base_type"] = this->base_type;
    nlohmann::json m_json;
    for (auto& m: this->methods) {
        m_json[m.first] = m.second->to_json();
    }
    j["methods"] = m_json;
    return nlohmann::json({{"type",      "typeclass"},
                           {"typeclass", j}});
}

TypeclassAst::TypeclassAst(const std::string& id, const std::string& base_type,
                     std::unordered_map<std::string, ast::UFunctionType> methods, TextPosition start, TextPosition end)
        : ast::TopNode(TopNodeType::TYPECLASS, start, end), id(id), base_type(base_type), methods(std::move(methods)) {
}

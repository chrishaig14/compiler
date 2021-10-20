//
// Created by chris on 1/8/21.
//

#include "Instance.h"

using namespace ast;

Instance::Instance(const std::string& id, ast::UObjectType base_type, std::unordered_map<std::string, ast::UFunctionNode> methods,
                   TextPosition start, TextPosition end) : ast::TopNode(TopNodeType::INSTANCE, start, end), id(id),
                                                           base_type(std::move(base_type)),
                                                           methods(std::move(methods)) {

}

bool Instance::equal(const ast::TopNode& other) const {
    return false;
}

nlohmann::json Instance::to_json() const {
    nlohmann::json j;
    j["id"] = this->id;
    j["base_type"] = this->base_type->to_json();
    nlohmann::json m_json;
    for (auto& m: this->methods) {
        m_json[m.first] = m.second->to_json();
    }
    j["methods"] = m_json;
    return nlohmann::json({{"type",     "instance"},
                           {"instance", j}});
}

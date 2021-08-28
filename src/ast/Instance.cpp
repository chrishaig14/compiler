//
// Created by chris on 1/8/21.
//

#include "Instance.h"

using namespace ast;

Instance::Instance(const std::string& id, UObjectType base_type,
                   const std::unordered_map<std::string, ast::Function*>& methods, TextPosition start, TextPosition end)
        : ast::Node(NodeType::INSTANCE, start, end), id(id), base_type(std::move(base_type)), methods(methods) {

}

bool Instance::equal(const ast::Node& other) const {
    return false;
}

nlohmann::json Instance::to_json() const {
    nlohmann::json j;
    j["id"] = this->id;
    j["base_type"] = this->base_type->to_json();
    nlohmann::json m_json;
    for (auto m: this->methods) {
        m_json[m.first] = m.second->to_json();
    }
    j["methods"] = m_json;
    return nlohmann::json({{"type",     "instance"},
                           {"instance", j}});
}

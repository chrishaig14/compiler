//
// Created by chris on 4/10/20.
//

#include "ClassNode.h"

ClassNode::ClassNode(const std::string& className, VectorOfStrings type_parameters, std::vector<std::pair<std::string,TypeNode*>> members,
                     std::unordered_map<std::string, Method> functions,
                     std::map<std::string, std::pair<TypeNode*, Node*>> static_members,
                     std::unordered_map<std::string, FunctionNode*> static_methods, TextPosition start,
                     TextPosition end) : Node(NodeType::CLS, start, end), members(members),
                                         static_members(static_members), methods(functions),
                                         static_methods(static_methods), class_name(className) {
    this->type_parameters = type_parameters;
}

bool ClassNode::equal(const Node& other) const {
    return false;
}


ClassNode::~ClassNode() {
    // for (const auto& mem: this->members) {
    //     delete mem.second;
    // }
    // for (const auto& method: this->methods) {
    //     delete method.second;
    // }
}

nlohmann::json ClassNode::to_json() {
    nlohmann::json j;
    j["type"] = "class";
    std::vector<nlohmann::json> memj;
    for (auto& i : this->members) {
        memj.push_back({{"id",   i.first},
                        {"type", i.second->to_json()}});
    }
    nlohmann::json methj;
    for (auto m: this->methods) {
        methj[m.first] = m.second.method->to_json();
    }
    nlohmann::json smethj;
    for (auto m: this->static_methods) {
        smethj[m.first] = m.second->to_json();
    }
    j["class"] = {{"id",             this->class_name},
                  {"members",        memj},
                  {"methods",        methj},
                  {"static_methods", smethj}};
    return j;
}

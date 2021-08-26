//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include <map>
#include "Node.h"

#include "TypeNode.h"
#include "FunctionNode.h"
#include "../macros.h"
#include "ast.h"
struct Method {
    Implicit* constraint;
    FunctionNode* method;
};

class ast::ClassNode : public Node {
public:

    bool equal(const Node& other) const override;
    ClassNode(const std::string& className, VectorOfStrings type_parameters,
              std::vector<std::pair<std::string, UTypeNode>> members, std::unordered_map<std::string, Method> functions,
              std::map<std::string, std::pair<TypeNode*, Node*>> static_members,
              std::unordered_map<std::string, UFunctionNode>& static_methods, TextPosition start, TextPosition end);
    ~ClassNode() override;
    nlohmann::json to_json() const override;

    std::vector<std::pair<std::string, UTypeNode>> members;
    std::map<std::string, std::pair<TypeNode*, Node*>> static_members;
    VectorOfStrings members_ordered;
    std::unordered_map<std::string, Method> methods;
    std::unordered_map<std::string, UFunctionNode> static_methods;

    std::string class_name;
    VectorOfStrings type_parameters;
};


#endif //CLASSNODE_H

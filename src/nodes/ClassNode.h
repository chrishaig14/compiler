//
// Created by chris on 4/10/20.
//

#ifndef CLASSNODE_H
#define CLASSNODE_H


#include "Node.h"

#include "TypeNode.h"
#include "FunctionNode.h"
#include "../macros.h"

class ClassNode : public Node {
public:

    bool equal(const Node& other) const;
    ClassNode(const std::string& className, VectorOfStrings type_parameters, MapStringType members,
              std::unordered_map<std::string, FunctionNode*> functions, MapStringType static_members,
              std::unordered_map<std::string, FunctionNode*> static_methods, TextPosition start, TextPosition end);
    ClassNode& cls() override;
    const ClassNode& cls() const override;

    ~ClassNode();

    MapStringType members;
    MapStringType static_members;
    VectorOfStrings members_ordered;
    std::unordered_map<std::string, FunctionNode*> methods;
    std::unordered_map<std::string, FunctionNode*> static_methods;

    std::string class_name;
    VectorOfStrings type_parameters;
};


#endif //CLASSNODE_H

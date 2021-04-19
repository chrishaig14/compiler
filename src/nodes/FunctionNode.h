//
// Created by chris on 1/8/20.
//

#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H


#include <string>
#include <vector>
#include "Node.h"

#include "TypeNode.h"
#include "BlockNode.h"

class FunctionNode : public Node {
public:
    std::string identifier;
    VectorOfStrings parameter_names;
    VectorOfTypes parameter_types;
    BlockNode* body;
    TypeNode* return_type;
    FunctionNode(std::string identifier, VectorOfStrings parameter_names, VectorOfTypes parameter_types,
                 TypeNode* return_type, BlockNode* body, TextPosition start, TextPosition end);

    bool equal(const Node& x) const override;

    ~FunctionNode();

    json to_json() const override;

    FunctionNode& func() override;

    const FunctionNode& func() const override;

    std::unordered_map<std::string, std::string> constraints;
    Path path;
    ConstFunction* const_function;
};


#endif //FUNCTIONNODE_H

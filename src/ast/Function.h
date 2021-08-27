//
// Created by chris on 1/8/20.
//

#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H


#include <string>
#include <vector>
#include "Node.h"

#include "TypeNode.h"
#include "Block.h"
#include "Path.h"
#include "ast.h"

struct Implicit {
    std::string type;
    std::string method;
    FunctionType* ft;
    bool is_static;
};

class ast::Function : public ast::Node {
public:
    Implicit* implicit;
    std::string identifier;
    VectorOfStrings parameter_names;
    VectorOfUTypes parameter_types;
    std::unique_ptr<ast::Block> body;
    UTypeNode return_type;
    Function(std::string identifier, const VectorOfStrings& parameter_names, VectorOfUTypes& parameter_types,
             UTypeNode& return_type, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end);

    bool equal(const ast::Node& x) const override;

    ~Function() override;
    nlohmann::json to_json() const override;

    std::unordered_map<std::string, std::string> constraints;
    Path path;
    ConstFunction* const_function;
};


#endif //FUNCTIONNODE_H

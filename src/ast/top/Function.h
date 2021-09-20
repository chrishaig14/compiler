//
// Created by chris on 1/8/20.
//

#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H


#include <string>
#include <vector>
#include "../Node.h"

#include "../Type.h"
#include "../Block.h"
#include "../Path.h"
#include "../ast.h"
#include "TopNode.h"

struct Implicit {
    std::string type;
    std::string method;
    ast::FunctionType* ft;
    bool is_static;
};

class ast::Function : public ast::TopNode {
    ast::VectorOfUTypes _parameter_types;
public:
    std::vector<std::reference_wrapper<ast::Type>> parameter_types;
    Implicit* implicit;
    std::string identifier;
    VectorOfStrings parameter_names;
    std::unique_ptr<ast::Block> body;
    ast::UTypeNode return_type;
    Function(std::string identifier, const VectorOfStrings& parameter_names, ast::VectorOfUTypes& parameter_types,
             ast::UTypeNode& return_type, std::unique_ptr<ast::Block>& body, TextPosition start, TextPosition end);

    bool equal(const ast::TopNode& x) const override;

    ~Function() override;
    nlohmann::json to_json() const override;

    std::unordered_map<std::string, std::string> constraints;
    Path path;
    // ConstFunction* const_function;
};


#endif //FUNCTIONNODE_H

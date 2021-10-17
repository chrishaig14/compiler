//
// Created by chris on 1/8/20.
//

#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H


#include <string>
#include <vector>
#include "../statements/Statement.h"

#include "../general/Type.h"
#include "../statements/Block.h"
#include "../../util/Path.h"
#include "../ast.h"
#include "TopNode.h"

class ast::Function : public ast::TopNode {
    ast::VectorOfUTypes _parameter_types;
public:
    std::vector<std::reference_wrapper<ast::Type>> parameter_types;
    std::string identifier;
    VectorOfStrings parameter_names;
    std::unique_ptr<ast::Block> body;
    ast::UTypeNode return_type;
    Function(std::string identifier, const VectorOfStrings& parameter_names, ast::VectorOfUTypes& parameter_types,
             ast::UTypeNode& return_type, std::unique_ptr<ast::Block>& body, std::unordered_map<std::string, std::string> constraints, TextPosition start, TextPosition end);

    bool equal(const ast::TopNode& x) const override;

    ~Function() override;
    nlohmann::json to_json() const override;

    std::unordered_map<std::string, std::string> constraints;
    Path path;
    // ConstFunction* const_function;

    void set_constraint(std::string gen_type_s, std::string typeclass_name_s);

    bool has_constraints() const;

    std::string gen_type;
    std::string typeclass_name;
};


#endif //FUNCTIONNODE_H

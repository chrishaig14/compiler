//
// Created by chris on 7/12/20.
//

#ifndef UNTITLED1_PARTIALAPPLICATION_H
#define UNTITLED1_PARTIALAPPLICATION_H

#include "../../../types.h"
#include "ExpNode.h"
#include "../../Type.h"
#include "../../ast.h"

class ast::PartialApplication : public ast::ExpNode {
public:
    ast::UExpNode function;
    ast::VectorOfExpNodesU args;
    ast::FunctionType* complete_type;
    PartialApplication(ast::UExpNode function, ast::VectorOfExpNodesU args, TextPosition start, TextPosition end);

    bool equal(const ast::ExpNode& other) const override;
    nlohmann::json to_json() const override;

};


#endif //UNTITLED1_PARTIALAPPLICATION_H

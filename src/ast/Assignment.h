//
// Created by chris on 1/8/20.
//

#ifndef ASSIGNMENTNODE_H
#define ASSIGNMENTNODE_H


#include "Node.h"

#include "Node.h"
#include "Type.h"
#include "ast.h"

class ast::Assignment : public ast::Node {
    ast::UExpNode _lvalue;
    ast::UExpNode _rvalue;
public:
    ExpNode& lvalue;
    ExpNode& rvalue;

    Assignment(ast::UExpNode lvalue, ast::UExpNode rvalue, TextPosition start, TextPosition end);

    bool equal(const ast::Node& x) const override;


    ~Assignment();
    nlohmann::json to_json() const override;

};


#endif //ASSIGNMENTNODE_H

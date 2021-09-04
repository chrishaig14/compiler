//
// Created by chris on 1/8/20.
//

#ifndef ASSIGNMENTNODE_H
#define ASSIGNMENTNODE_H


#include "Node.h"

#include "Node.h"
#include "TypeNode.h"
#include "ast.h"

class ast::Assignment : public ast::Node {
    UNode _lvalue;
    UNode _rvalue;
public:
    Node& lvalue;
    Node& rvalue;

    Assignment(UNode lvalue, UNode rvalue, TextPosition start, TextPosition end);

    bool equal(const ast::Node& x) const override;


    ~Assignment();
    nlohmann::json to_json() const override;

};


#endif //ASSIGNMENTNODE_H

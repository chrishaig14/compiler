//
// Created by chris on 7/11/20.
//

#ifndef CONTINUENODE_H
#define CONTINUENODE_H


#include "Node.h"
#include "ast.h"

class ast::ContinueNode : public ast::Node {
public:
    ContinueNode(TextPosition start, TextPosition end);
    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;
};


#endif //CONTINUENODE_H

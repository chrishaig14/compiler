//
// Created by chris on 24/3/21.
//

#ifndef XLANG_CAST_H
#define XLANG_CAST_H

#include "Node.h"
#include "optypes.h"
#include "TypeNode.h"
#include "ast.h"
class ast::Cast : public ast::Node {
public:
    Node* exp;
    std::string as_type;


    Cast(Node* exp, std::string as_type, TextPosition start, TextPosition end);

    bool equal(const ast::Node& x) const override;

    ~Cast();

    TypeNode* exp_type;
    TypeNode* rtype;
};


#endif //XLANG_CAST_H

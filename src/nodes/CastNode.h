//
// Created by chris on 24/3/21.
//

#ifndef XLANG_CASTNODE_H
#define XLANG_CASTNODE_H

#include "Node.h"
#include "optypes.h"
#include "TypeNode.h"

class CastNode : public Node {
public:
    Node* exp;
    std::string as_type;

    CastNode& cast() override;

    const CastNode& cast() const override;;

    CastNode(Node* exp, std::string as_type, TextPosition start);

    bool equal(const Node& x) const override;

    ~CastNode();

    TypeNode* exp_type;
    TypeNode* rtype;
};


#endif //XLANG_CASTNODE_H

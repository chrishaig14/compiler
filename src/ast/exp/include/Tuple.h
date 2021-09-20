//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_TUPLENODE_H
#define UNTITLED1_TUPLENODE_H


#include "ExpNode.h"
#include "../../../util/types.h"

class ast::Tuple : public ast::ExpNode {
public:
    int tuple_index;
    ast::VectorOfExpNodesU values;
    bool equal(const ast::ExpNode& other) const override;
    Tuple(ast::VectorOfExpNodesU values, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;

};


#endif //UNTITLED1_TUPLENODE_H

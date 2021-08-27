//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_TUPLENODE_H
#define UNTITLED1_TUPLENODE_H


#include "Node.h"
#include "../types.h"

class ast::TupleNode : public ast::Node {
public:
    int tuple_index;
    VectorOfNodes values;
    bool equal(const ast::Node& other) const override;
    TupleNode(const VectorOfNodes& values, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;

};


#endif //UNTITLED1_TUPLENODE_H

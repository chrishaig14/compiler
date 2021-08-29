//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_TUPLENODE_H
#define UNTITLED1_TUPLENODE_H


#include "Node.h"
#include "../types.h"

class ast::Tuple : public ast::Node {
public:
    int tuple_index;
    VectorOfNodesU values;
    bool equal(const ast::Node& other) const override;
    Tuple(VectorOfNodesU values, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;

};


#endif //UNTITLED1_TUPLENODE_H

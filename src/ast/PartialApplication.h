//
// Created by chris on 7/12/20.
//

#ifndef UNTITLED1_PARTIALAPPLICATION_H
#define UNTITLED1_PARTIALAPPLICATION_H

#include "../types.h"
#include "Node.h"
#include "TypeNode.h"
#include "ast.h"

class ast::PartialApplication : public ast::Node {
public:
    UNode function;
    VectorOfNodes args;
    FunctionType* complete_type;
    PartialApplication(UNode function, VectorOfNodes args, TextPosition start, TextPosition end);

    bool equal(const ast::Node& other) const override;
    nlohmann::json to_json() const override;

};


#endif //UNTITLED1_PARTIALAPPLICATION_H

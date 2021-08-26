//
// Created by chris on 1/8/20.
//

#ifndef IDNODE_H
#define IDNODE_H


#include <string>
#include "Node.h"

#include "Node.h"
#include "../semantic/SymbolTable.h"

class IdNode : public Node {
public:
    std::string _id;
    bool is_global_function;

    IdNode(std::string identifier, TextPosition start, TextPosition end);

    static UIdNode make(std::string identifier, TextPosition start, TextPosition end) {
        return std::make_unique<IdNode>(identifier, start, end);
    }

    bool equal(const Node& other) const override;

    nlohmann::json to_json() const override;
};


#endif //IDNODE_H

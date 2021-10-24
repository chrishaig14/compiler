//
// Created by chris on 1/8/20.
//

#ifndef IDNODE_H
#define IDNODE_H


#include <string>
#include "ExpNode.h"

#include "ExpNode.h"
#include <semantic/SymbolTable.h>
#include "../../ast.h"

class ast::Id : public ast::ExpNode {
public:
    std::string _id;
    bool is_global_function;

    Id(std::string identifier, TextPosition start, TextPosition end);

    static UId make(std::string identifier, TextPosition start, TextPosition end) {
        return std::make_unique<Id>(identifier, start, end);
    }

    bool equal(const ast::ExpNode& other) const override;

    nlohmann::json to_json() const override;
};


#endif //IDNODE_H

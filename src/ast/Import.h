//
// Created by chris on 24/12/20.
//

#ifndef UNTITLED1_IMPORTNODE_H
#define UNTITLED1_IMPORTNODE_H


#include "Node.h"

#include <utility>
#include "../types.h"
#include "ast.h"

class ast::Import : public ast::Node {
public:
    bool has_alias;
    bool equal(const ast::Node& other) const override;

    Import(const VectorOfStrings& path, std::string alias, TextPosition start, TextPosition end)
            : ast::Node(NodeType::IMPORT, start, end) {
        this->alias = std::move(alias);
        this->path = path;
        this->has_alias = true;
    }

    Import(const VectorOfStrings& path, TextPosition start, TextPosition end) : ast::Node(NodeType::IMPORT,
                                                                                          start,
                                                                                          end) {
        this->has_alias = false;
        this->path = path;
    }

    nlohmann::json to_json() const override;

    VectorOfStrings path;
    std::string alias;
};


#endif //UNTITLED1_IMPORTNODE_H

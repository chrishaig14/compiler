//
// Created by chris on 1/8/20.
//

#ifndef STRINGNODE_H
#define STRINGNODE_H


#include <string>
#include "Node.h"
#include "ast.h"

class ast::String : public ast::Node {
public:
    std::string str;
    String(const std::string& str, TextPosition start, TextPosition end);

    bool equal(const ast::Node& x) const override;

    nlohmann::json to_json() const override;
};


#endif //STRINGNODE_H

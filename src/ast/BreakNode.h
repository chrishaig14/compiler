//
// Created by chris on 18/8/20.
//

#ifndef BREAKNODE_H
#define BREAKNODE_H


#include "Node.h"
#include "TypeNode.h"


class BreakNode : public Node {
public:
    std::vector<std::pair<std::string,TypeNode*>> loop_vars;
    BreakNode(TextPosition start, TextPosition end);
    bool equal(const Node& other) const override;
    nlohmann::json to_json() const override;

};


#endif //BREAKNODE_H

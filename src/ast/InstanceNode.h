//
// Created by chris on 1/8/21.
//

#ifndef XLANG_INSTANCENODE_H
#define XLANG_INSTANCENODE_H


#include "Node.h"
#include "ObjectType.h"
#include "Function.h"

class InstanceNode : public Node {
public:
    std::string id;
    ObjectType* base_type;
    std::unordered_map<std::string, ast::Function*> methods;
    InstanceNode(const std::string& id, ObjectType* base_type,
                 const std::unordered_map<std::string, ast::Function*>& methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    bool equal(const Node& other) const override;
};


#endif //XLANG_INSTANCENODE_H

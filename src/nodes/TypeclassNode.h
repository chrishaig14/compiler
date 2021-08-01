#ifndef XLANG_TYPECLASSNODE_H
#define XLANG_TYPECLASSNODE_H


#include "Node.h"
#include "FunctionType.h"

class TypeclassNode : public Node {
public:
    std::string id;
    std::string base_type;
    std::unordered_map<std::string, FunctionType*> methods;
    TypeclassNode(const std::string& id, const std::string& base_type,
                  const std::unordered_map<std::string, FunctionType*>& methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() override;
    bool equal(const Node& other) const override;
};


#endif //XLANG_TYPECLASSNODE_H

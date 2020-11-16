//
// Created by chris on 1/8/20.
//

#include "TypeNode.h"

FunctionTypeNode::FunctionTypeNode(const std::vector<TypeNode>& parameterTypes,
                                   TypeNode returnType)
        : parameter_types(parameterTypes), return_type(returnType) {}

ObjectTypeNode::ObjectTypeNode(const std::string& identifier,
                               const std::vector<TypeNode>& typeParameters) : identifier(
        identifier), type_parameters(typeParameters) {

}
std::string ftype_to_string(const FunctionTypeNode& ftype) {
    std::string parameters;
    std::string ret;
    for (auto p: ftype.parameter_types) {
        parameters += p.to_string() + ", ";
    }
    if (ftype.parameter_types.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
    }
    ret = ftype.return_type.to_string() + ", ";
    return "fun (" + parameters + ") . " + ret;
}

std::string otype_to_string(const ObjectTypeNode& otype) {
    std::string parameters;
    for (auto p: otype.type_parameters) {
        parameters += p.to_string() + ", ";
    }
    if (parameters.size() != 0) {
        parameters = parameters.substr(0, parameters.size() - 2);
        return otype.identifier + "[" + parameters + "]";
    }
    return otype.identifier;
}

bool ftype_equal(const FunctionTypeNode& a,const  FunctionTypeNode& b) {
    if (a.parameter_types.size() != b.parameter_types.size()) return false;
    for (int i = 0; i < a.parameter_types.size(); i++) {
        if (a.parameter_types[i] != b.parameter_types[i]) return false;
    }
    return a.return_type == b.return_type;
}

bool otype_equal(const ObjectTypeNode& a,const  ObjectTypeNode& b) {
    if (a.identifier != b.identifier) return false;
    if (a.type_parameters.size() != b.type_parameters.size()) return false;
    for (int i = 0; i < a.type_parameters.size(); i++) {
        if (a.type_parameters[i] != b.type_parameters[i]) return false;
    }
    return true;
}
//
// Created by chris on 28/6/20.
//

#ifndef UNTITLED1_FIRSTPASS_H
#define UNTITLED1_FIRSTPASS_H


#include <map>
#include "AstNode.h"

class SimpleInfo;

class FunctionInfo;

class SimpleInfo {
public:
    std::string parent;
    std::vector<SimpleInfo*> type_parameters;

    SimpleInfo(TypeNode* n);
};

class FunctionInfo {
public:
    std::vector<SimpleInfo*> parameter_types;
    SimpleInfo* return_type;

    FunctionInfo(std::vector<TypeNode*> parameter_types, TypeNode* return_type);
};

class ClassInfo {
public:
    std::map<std::string, SimpleInfo*> fields;
    std::map<std::string, FunctionInfo*> methods;
};

enum class GINFO {
    CLASS, FUNCTION, SIMPLE
};

class GeneralInfo {
public:
    union {
        ClassInfo* class_info;
        FunctionInfo* function_info;
        SimpleInfo* simple_info;
    };
    GINFO type;
};

class FirstPass {
public:
    std::map<std::string, GeneralInfo*> globals;

    void analyze(ClassNode* node);

    void analyze(FunctionNode* node);

    void analyze(VectorOfNodes program);

};

#endif //UNTITLED1_FIRSTPASS_H

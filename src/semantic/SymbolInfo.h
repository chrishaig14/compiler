//
// Created by chris on 2/8/20.
//

#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include "FunctionInfo.h"
#include "ObjectInfo.h"
#include "ClassInfo.h"

class SymbolInfo {
    const TypeNode* _type;
public:
    bool is_function;
    bool is_method;
    ClassInfo* class_info;
    bool is_class_method;

    SymbolInfo();

    void set_type(const TypeNode& typ);

    const TypeNode& type();
};


#endif //SYMBOLINFO_H

//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_SYMBOLINFO_H
#define UNTITLED1_SYMBOLINFO_H

#include "FunctionInfo.h"
#include "ObjectInfo.h"

enum class SINFO {
    CLASS, FUNCTION, SIMPLE
};

class SymbolInfo {
public:
    union {
        FunctionInfo* function_info;
        ObjectInfo* object_info;
    };
    SINFO type;

    SymbolInfo(FunctionInfo* function_info) : function_info(function_info) {
        this->type = SINFO::FUNCTION;
    }

    SymbolInfo(ObjectInfo* object_info) : object_info(object_info) {
        this->type = SINFO::SIMPLE;
    }

    SymbolInfo() {}

    bool is_function() { return this->type == SINFO::FUNCTION; }

    bool is_object() { return this->type == SINFO::SIMPLE; }

    virtual ~SymbolInfo() {
        if (this->is_function()) delete this->function_info;
        if (this->is_object()) delete this->object_info;

    }

    bool operator!=(const SymbolInfo& b) const {
        return !(*this == b);
    }

    bool operator==(const SymbolInfo& b) const {
        if (this->type == b.type) {
            switch (this->type) {
                case SINFO::FUNCTION:
                    return *this->function_info == *b.function_info;
                case SINFO::SIMPLE:
                    return *this->object_info == *b.object_info;
            }
        }
        return false;
    }


};

#endif //UNTITLED1_SYMBOLINFO_H

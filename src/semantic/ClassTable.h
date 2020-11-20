//
// Created by chris on 18/7/20.
//

#ifndef CLASSTABLE_H
#define CLASSTABLE_H


#include <map>
#include "SymbolTable.h"
#include "ClassInfo.h"

class ClassTable {
    std::map<std::string, ClassInfo*> table;
public:
    ClassInfo* get(std::string name);

    bool declared(std::string name);

    void set(std::string name, ClassInfo* info);
};

#endif //CLASSTABLE_H

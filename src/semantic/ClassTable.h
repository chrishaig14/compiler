//
// Created by chris on 18/7/20.
//

#ifndef CLASSTABLE_H
#define CLASSTABLE_H


#include <unordered_map>
#include "SymbolTable.h"
#include "ClassInfo.h"

class ClassTable {
    std::unordered_map<std::string, ClassInfo*> table;
public:
    ClassInfo* get(std::string name);

    bool declared(std::string name);

    void set(std::string name, ClassInfo* info);

    ~ClassTable() {
        for (auto c: this->table) {
            delete c.second;
        }
    }
};

#endif //CLASSTABLE_H

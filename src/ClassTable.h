//
// Created by chris on 18/7/20.
//

#ifndef UNTITLED1_CLASSTABLE_H
#define UNTITLED1_CLASSTABLE_H


#include <map>
#include "SymbolTable.h"

class ClassTable {
    std::map<std::string, ClassInfo> table;
public:
    ClassInfo get(std::string name) {
        if (this->table.count(name) == 1) {
            return this->table[name];
        }
        throw std::runtime_error("Symbol " + name + " not found in scope");
    }

    bool declared(std::string name) {
        return this->table.count(name) == 1;
    }

    void set(std::string name, ClassInfo info) {
        this->table[name] = info;
    }
};

#endif //UNTITLED1_CLASSTABLE_H

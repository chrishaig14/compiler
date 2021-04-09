//
// Created by chris on 18/7/20.
//

#include "ClassTable.h"

Class* ClassTable::get(std::string name) {
    if (this->table.find(name) != this->table.end()) {
        return this->table[name];
    }
    throw std::runtime_error("Symbol " + name + " not found in scope");
}

bool ClassTable::declared(std::string name) {
    return this->table.find(name) != this->table.end();
}

void ClassTable::set(std::string name, Class* info) {
    this->table[name] = info;
}

ClassTable::~ClassTable() {
    std::cout << "Delete ClassTable "<<this << std::endl;
    for (auto c: this->table) {
        delete c.second;
    }
}

ClassTable::ClassTable() {
    std::cout << "New ClassTable "<<this << std::endl;
}

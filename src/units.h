//
// Created by chris on 4/4/21.
//

#ifndef XLANG_UNITS_H
#define XLANG_UNITS_H
#include "nodes/BlockNode.h"
#include "scanner/CodeLines.h"

enum class UnitType {
    PACKAGE, MODULE
};

class Unit {
public:
    std::string name;
    UnitType type;
    std::string path;
    std::string dotted_path;
    Unit(std::string name, std::string path, UnitType type);
};

class Package : public Unit {
public:
    std::map<std::string, Unit*> children;

    Package(std::string name, std::string path);
};

class Module : public Unit {
    std::map<std::string, std::string> children;
public:
    Module(std::string name, std::string path);

    BlockNode* ast;
    std::map<std::string, std::string> imported_paths;
    std::string dotted_path;
    CodeLines code_lines;
    std::map<std::string, std::string> local_paths;
};

#endif //XLANG_UNITS_H

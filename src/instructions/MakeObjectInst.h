//
// Created by chris on 5/8/20.
//

#ifndef UNTITLED1_MAKEOBJECTINST_H
#define UNTITLED1_MAKEOBJECTINST_H


#include <string>
#include <vector>
#include "Instruction.h"

class MakeObjectInst : public Instruction {
public:
    MakeObjectInst(std::string type, std::vector<std::string> fields) {
        this->type = type;
        this->fields = fields;
    }

public:
    bool equal(const Instruction* inst) const override {
        return true;
    }

    std::string to_string() const override {
        std::string st = "MAKE_OBJECT " + this->type;
        return st;
    }

    void accept(InstructionVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string type;
    std::vector<std::string> fields;
};


#endif //UNTITLED1_MAKEOBJECTINST_H

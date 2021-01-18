//
// Created by chris on 5/8/20.
//

#ifndef MAKEOBJECTINST_H
#define MAKEOBJECTINST_H


#include <string>
#include <vector>
#include "Instruction.h"
#include "../types.h"

class MakeObjectInst : public Instruction {
public:
    MakeObjectInst(std::string type, VectorOfStrings fields) {
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
    VectorOfStrings fields;
};


#endif //MAKEOBJECTINST_H

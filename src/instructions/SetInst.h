//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_SETINST_H
#define UNTITLED1_SETINST_H


#include <string>
#include "Instruction.h"

class SetInst : public Instruction {
public:
    SetInst(const std::string& identifier);

    bool equal(const Instruction* inst) const {
        const SetInst* other = dynamic_cast<const SetInst*>(inst);
        if (other == nullptr) return false;
        return this->identifier == other->identifier;
    }

    std::string to_string() const {
        return "SET " + this->identifier;
    }

private:
    std::string identifier;

};


#endif //UNTITLED1_SETINST_H

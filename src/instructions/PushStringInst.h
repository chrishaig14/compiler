//
// Created by chris on 2/8/20.
//

#ifndef PUSHSTRINGINST_H
#define PUSHSTRINGINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class PushStringInst : public Instruction {
public:
    PushStringInst(const std::string& str);

    bool equal(const Instruction* inst) const {
        const PushStringInst* other = dynamic_cast<const PushStringInst*>(inst);
        if (other == nullptr) return false;
        return this->str == other->str;
    }

    std::string to_string() const {
        return "PUSH_STRING \"" + this->str + "\"";
    }


    void accept(InstructionVisitor& visitor);

    std::string str;

};


#endif //PUSHSTRINGINST_H

//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_GETSUBSCRIPTINST_H
#define UNTITLED1_GETSUBSCRIPTINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class GetSubscriptInst : public Instruction {
    bool equal(const Instruction* inst) const {
        const GetSubscriptInst* other = dynamic_cast<const GetSubscriptInst*>(inst);
        if (other == nullptr) return false;
        return true;
    }

    std::string to_string() const {
        return "GET_SUBSCRIPT";
    }

    void visit(InstructionVisitor& visitor);
};


#endif //UNTITLED1_GETSUBSCRIPTINST_H

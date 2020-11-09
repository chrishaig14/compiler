//
// Created by chris on 2/8/20.
//

#ifndef GETSUBSCRIPTINST_H
#define GETSUBSCRIPTINST_H


#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"

class GetSubscriptInst : public Instruction {
    bool equal(const Instruction* inst) const {
        const GetSubscriptInst* other = dynamic_cast<const GetSubscriptInst*>(inst);
        if (other == nullptr) return false;
        return true;
    }

    void accept(InstructionVisitor& visitor);

public:
    std::string to_string() const {
        return "GET_SUBSCRIPT";
    }
};


#endif //GETSUBSCRIPTINST_H

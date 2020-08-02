//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_SETSUBSCRIPTINST_H
#define UNTITLED1_SETSUBSCRIPTINST_H


#include <string>
#include "Instruction.h"

class SetSubscriptInst : public Instruction {
    bool equal(const Instruction* inst) const {
        const SetSubscriptInst* other = dynamic_cast<const SetSubscriptInst*>(inst);
        if (other == nullptr) return false;
        return true;
    }

    std::string to_string() const {
        return "SET_SUBSCRIPT";
    }
};


#endif //UNTITLED1_SETSUBSCRIPTINST_H

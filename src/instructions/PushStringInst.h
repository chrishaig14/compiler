//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_PUSHSTRINGINST_H
#define UNTITLED1_PUSHSTRINGINST_H


#include <string>
#include "Instruction.h"

class PushStringInst : public Instruction {
public:
    PushStringInst(const std::string& str);

    bool equal(const Instruction* inst) const {
        const PushStringInst* other = dynamic_cast<const PushStringInst*>(inst);
        if (other == nullptr) return false;
        return this->str == other->str;
    }

    std::string to_string() const {
        return "PUSH_STRING " + this->str;
    }

private:
    std::string str;

};


#endif //UNTITLED1_PUSHSTRINGINST_H

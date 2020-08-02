//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_PUSHINTEGERINST_H
#define UNTITLED1_PUSHINTEGERINST_H

#include <string>
#include "Instruction.h"

class PushIntegerInst : public Instruction {
public:
    PushIntegerInst(int num);

    bool equal(const Instruction* inst) const {
        const PushIntegerInst* other = dynamic_cast<const PushIntegerInst*>(inst);
        if (other == nullptr) return false;
        return this->num == other->num;
    }

    std::string to_string() const {
        return "PUSH_INTEGER " + std::to_string(this->num);
    }

private:
    int num;
};


#endif //UNTITLED1_PUSHINTEGERINST_H

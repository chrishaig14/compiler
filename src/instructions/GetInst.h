//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_GETINST_H
#define UNTITLED1_GETINST_H


#include <string>
#include "Instruction.h"

class GetInst : public Instruction {
public:
    GetInst(const std::string& identifier);

public:
    std::string identifier;

    bool equal(const Instruction* inst) const {
        const GetInst* other = dynamic_cast<const GetInst*>(inst);
        if (other == nullptr) return false;
        return this->identifier == other->identifier;
    }

    std::string to_string() const override;


};


#endif //UNTITLED1_GETINST_H

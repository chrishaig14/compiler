//
// Created by chris on 18/8/20.
//

#ifndef LEAVESCOPE_H
#define LEAVESCOPE_H


#include "Instruction.h"

class LeaveScope : public Instruction {
public:
    std::string name;

    LeaveScope(const std::string& name);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //LEAVESCOPE_H

//
// Created by chris on 2/8/20.
//

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include "InstructionVisitor.h"

enum class InstType {
    OTHER, START_FUNCTION, END_FUNCTION
};

class Instruction {
public:
    InstType type = InstType::OTHER;

    virtual bool equal(const Instruction* inst) const = 0;

    virtual std::string to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Instruction& inst) {
        return os << inst.to_string();
    };

    virtual void accept(InstructionVisitor& visitor) = 0;

};


#endif //INSTRUCTION_H
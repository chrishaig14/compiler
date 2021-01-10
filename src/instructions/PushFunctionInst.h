//
// Created by chris on 2/8/20.
//

#ifndef PUSHFUNCTIONINST_H
#define PUSHFUNCTIONINST_H


#include <vector>
#include <string>
#include "Instruction.h"
#include "InstructionVisitor.h"
#include "../types.h"

class PushFunctionInst : public Instruction {
public:
    PushFunctionInst(const std::vector<std::string>& parameterNames, const Code& body);

    Code body;
    std::vector<std::string> free_variables;
    std::vector<std::string> parameter_names;
public:

    bool equal(const Instruction* inst) const {
        const PushFunctionInst* other = dynamic_cast<const PushFunctionInst*>(inst);
        if (other == nullptr) {
            return false;
        }
        return *this == *other;
    }

    bool operator==(const PushFunctionInst& other) const {
        if (this->parameter_names != other.parameter_names) {
            return false;
        }
        if (this->free_variables != other.free_variables) {
            return false;
        }
        if (this->body != other.body) {
            return false;
        }
        return true;
    }

    std::string to_string() const {
        return "PUSH_FUNCTION";
    }

    void accept(InstructionVisitor& visitor);

};


#endif //PUSHFUNCTIONINST_H

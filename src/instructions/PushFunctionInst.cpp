//
// Created by chris on 2/8/20.
//

#include "PushFunctionInst.h"

PushFunctionInst::PushFunctionInst(const std::vector<std::string>& parameterNames, const Code& body) : parameter_names(
        parameterNames),
                                                                                                       body(body) {}

void PushFunctionInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

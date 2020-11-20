//
// Created by chris on 18/8/20.
//

#include "LabelledCode.h"

LabelledCode::LabelledCode(std::vector<std::pair<std::string, Instruction*>> code) {
    for (int i = 0; i < code.size(); i++) {
        if (code[i].first.size() != 0) {
            this->labels[code[i].first] = i;
        }
    }
//        now we have for each label its absolute offset
}

int LabelledCode::get_label(std::string label) {
    return this->labels[label];
}

Instruction* LabelledCode::get_instruction(int address) {
    return this->code[address].second;
}

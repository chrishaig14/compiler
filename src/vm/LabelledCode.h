//
// Created by chris on 18/8/20.
//

#ifndef UNTITLED1_LABELLEDCODE_H
#define UNTITLED1_LABELLEDCODE_H


#include <map>
#include <vector>
#include "../instructions/Instruction.h"
#include "../instructions/JumpInst.h"
#include "../instructions/JumpIfFalseInst.h"

class LabelledCode {
public:
    std::map<std::string, int> labels;
    std::vector<std::pair<std::string, Instruction*>> code;

    LabelledCode(std::vector<std::pair<std::string, Instruction*>> code) {
        for (int i = 0; i < code.size(); i++) {
            if (code[i].first.size() != 0) {
                this->labels[code[i].first] = i;
            }
        }
//        now we have for each label its absolute offset
    }

//    std::vector<Instruction*> relativize() {
//        for (int current_address = 0; current_address < this->code.size(); current_address++) {
//            Instruction* inst = code[current_address].second;
//            JumpInst* jump = dynamic_cast<JumpInst*>(inst);
//            if (jump != nullptr) {
//                // It's an unconditional jump
//                int absolute_address = this->labels[jump->label];
//                int relative_address = absolute_address - current_address;
//                jump->label = "";
//                jump->offset = relative_address;
//            } else {
//                JumpIfFalseInst* jump = dynamic_cast<JumpIfFalseInst*>(inst);
//                if (jump != nullptr) {
//                    // It's an conditional jump
//                    int absolute_address = this->labels[jump->label];
//                    int relative_address = absolute_address - current_address;
//                    jump->label = "";
//                    jump->offset = relative_address;
//                }
//            }
//        }
//    }

    int get_label(std::string label) {
        return this->labels[label];
    }

    Instruction* get_instruction(int address) {
        return this->code[address].second;
    }
};


#endif //UNTITLED1_LABELLEDCODE_H

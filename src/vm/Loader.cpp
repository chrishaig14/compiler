//
// Created by chris on 23/8/20.
//

#include "Loader.h"
#include "Environment.h"
#include "CodeObject.h"
#include "../instructions/StartFunction.h"
#include "../instructions/JumpInst.h"
#include "../instructions/JumpIfFalseInst.h"
#include "../instructions/JumpIfNone.h"


void Loader::load() {
    CodeLabel current_function_code;
    std::string current_function_name;
    for (int i = 0; i < this->all_code.size(); i++) {
        std::pair<std::string, Instruction*> label_inst = all_code[i];
        Instruction* inst = label_inst.second;
        switch (inst->type) {
            case InstType::START_FUNCTION: {
                current_function_code = {};
                StartFunction* start_function_inst = dynamic_cast<StartFunction*>(inst);
                if (start_function_inst == nullptr) { throw std::runtime_error("instruction type doesn't match!"); }
                current_function_name = start_function_inst->name;
                break;
            }
            case InstType::END_FUNCTION: {
                this->load_function(current_function_name, current_function_code);
                break;
            }
            default:
                current_function_code.push_back(label_inst);
        }
    }
}

void Loader::load_function(std::string name, CodeLabel code) {
    std::map<std::string, int> labels;
    for (int i = 0; i < code.size(); i++) {
        if (code[i].first != "") {
            labels[code[i].first] = i;
        }
    }
    for (int i = 0; i < code.size(); i++) {
        JumpInst* jinst = dynamic_cast<JumpInst*>(code[i].second);
        if (jinst != nullptr) {
            if (jinst->offset == 0) {
                jinst->offset = labels[jinst->label] - i;
                jinst->label = "";
            }
        } else {
            JumpIfFalseInst* jfalse = dynamic_cast<JumpIfFalseInst*>(code[i].second);
            if (jfalse != nullptr) {
                if (jfalse->offset == 0) {
                    jfalse->offset = labels[jfalse->label] - i;
                    jfalse->label = "";
                }
            } else {
                JumpIfNone* jnone = dynamic_cast<JumpIfNone*>(code[i].second);
                if (jnone != nullptr) {
                    if (jnone->offset == 0) {
                        jnone->offset = labels[jnone->label] - i;
                        jnone->label = "";
                    }
                }
            }
        }
    }
    Code code_nl;
    for (int i = 0; i < code.size(); i++) {
        code_nl.push_back(code[i].second);
    }
    CodeUser* function_object = new CodeUser(code_nl);
    this->global_env->declare(name);
    this->global_env->set(name, new CodeObject(function_object));
}

Loader::Loader(CodeLabel allCode) : all_code(allCode) {
    this->global_env = new Environment("global", nullptr);
}

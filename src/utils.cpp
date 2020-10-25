//
// Created by chris on 5/9/20.
//

#include "utils.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, CodeLabel v) {
    for (auto i: v) {
        std::string spaces;
        for (int k = i.first.size(); k < 32; k++) {
            spaces += " ";
        }
        out << i.first + spaces + i.second->to_string() << std::endl;
    }
    return out;
}

bool operator==(CodeLabel a, CodeLabel b) {
    if (a.size() != b.size()) {
        int min = a.size() < b.size() ? a.size() : b.size();
        for (int i = 0; i < min; i++) {
            auto a_inst = a[i].second;
            auto b_inst = b[i].second;
            std::cerr << "comparing " << a_inst->to_string() << " == " << b_inst->to_string() << ": "
                      << (a_inst->equal(b_inst) ? "True" : "False") << std::endl;
        }
        return false;
    }
    for (int i = 0; i < a.size(); i++) {
        auto a_inst = a[i].second;
        auto b_inst = b[i].second;
        std::cerr << "comparing " << a_inst->to_string() << " == " << b_inst->to_string() << ": "
                  << (a_inst->equal(b_inst) ? "True" : "False") << std::endl;
        if (!a_inst->equal(b_inst)) return false;
    }
    return true;
}



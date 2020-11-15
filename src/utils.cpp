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


bool operator==(const NodeContainer& a, const NodeContainer& b) {
    if (a.ntype != b.ntype) {
        return false;
    }
    switch (a.ntype) {
        case NodeType::ASSIGN:
            return a.assign() == b.assign();
            break;
        case NodeType::BINOP:
            return a.binop() == b.binop();
            break;
        case NodeType::BLOCK:
            return a.block() == b.block();
            break;
        case NodeType::BOOLEAN:
            return a.boolean() == b.boolean();
            break;
        case NodeType::BRK:
            return a.brk() == b.brk();
            break;
        case NodeType::CALL:
            return a.call() == b.call();
            break;
        case NodeType::CLSEXP:
            return a.clsexp() == b.clsexp();
            break;
        case NodeType::CLSFLD:
            return a.clsfld() == b.clsfld();
            break;
        case NodeType::CLS:
            return a.cls() == b.cls();
            break;
        case NodeType::CNTINUE:
            return a.cntinue() == b.cntinue();
            break;
        case NodeType::DECL:
            return a.decl() == b.decl();
            break;
        case NodeType::EMPTYLST:
            return a.emptylst() == b.emptylst();
            break;
        case NodeType::FORLOOP:
            return a.forloop() == b.forloop();
            break;
        case NodeType::FUNC:
            return a.func() == b.func();
            break;
        case NodeType::ID:
            return a.id() == b.id();
            break;
        case NodeType::IFF:
            return a.iff() == b.iff();
            break;
        case NodeType::INSTANCE:
//                return a.instance() == b.instance();
            break;
        case NodeType::LST:
            return a.lst() == b.lst();
            break;
        case NodeType::MEMBER:
            return a.member() == b.member();
            break;
        case NodeType::NONE:
            return a.none() == b.none();
            break;
        case NodeType::NUMBER:
            return a.number() == b.number();
            break;
        case NodeType::RETRN:
            return a.retrn() == b.retrn();
            break;
        case NodeType::STRNG:
            return a.strng() == b.strng();
            break;
        case NodeType::STRCT:
            return a.strct() == b.strct();
            break;
        case NodeType::SUB:
            return a.sub() == b.sub();
            break;
        case NodeType::TERNARY:
            return a.ternary() == b.ternary();
            break;
//        case NodeType::TYPE:
//                return a.type() == b.type();
//            break;
        case NodeType::WHIL:
            return a.whil() == b.whil();
            break;
        case NodeType::UNINITIALIZED:
            break;
        default:
            throw std::runtime_error("Dont know what to do here!");
    }
    return false;
}

bool operator!=(const NodeContainer& a, const NodeContainer& b) { return !(a == b); }

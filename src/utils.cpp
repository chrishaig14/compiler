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
        case NodeContainer::ASSIGN:
            return a.assign() == b.assign();
            break;
        case NodeContainer::BINOP:
            return a.binop() == b.binop();
            break;
        case NodeContainer::BLOCK:
            return a.block() == b.block();
            break;
        case NodeContainer::BOOLEAN:
            return a.boolean() == b.boolean();
            break;
        case NodeContainer::BRK:
            return a.brk() == b.brk();
            break;
        case NodeContainer::CALL:
            return a.call() == b.call();
            break;
        case NodeContainer::CLSEXP:
            return a.clsexp() == b.clsexp();
            break;
        case NodeContainer::CLSFLD:
            return a.clsfld() == b.clsfld();
            break;
        case NodeContainer::CLS:
            return a.cls() == b.cls();
            break;
        case NodeContainer::CNTINUE:
            return a.cntinue() == b.cntinue();
            break;
        case NodeContainer::DECL:
            return a.decl() == b.decl();
            break;
        case NodeContainer::EMPTYLST:
            return a.emptylst() == b.emptylst();
            break;
        case NodeContainer::FORLOOP:
            return a.forloop() == b.forloop();
            break;
        case NodeContainer::FUNC:
            return a.func() == b.func();
            break;
        case NodeContainer::ID:
            return a.id() == b.id();
            break;
        case NodeContainer::IFF:
            return a.iff() == b.iff();
            break;
        case NodeContainer::INSTANCE:
//                return a.instance() == b.instance();
            break;
        case NodeContainer::LST:
            return a.lst() == b.lst();
            break;
        case NodeContainer::MEMBER:
            return a.member() == b.member();
            break;
        case NodeContainer::NONE:
            return a.none() == b.none();
            break;
        case NodeContainer::NUMBER:
            return a.number() == b.number();
            break;
        case NodeContainer::RETRN:
            return a.retrn() == b.retrn();
            break;
        case NodeContainer::STRNG:
            return a.strng() == b.strng();
            break;
        case NodeContainer::STRCT:
            return a.strct() == b.strct();
            break;
        case NodeContainer::SUB:
            return a.sub() == b.sub();
            break;
        case NodeContainer::TERNARY:
            return a.ternary() == b.ternary();
            break;
//        case NodeContainer::TYPE:
//                return a.type() == b.type();
//            break;
        case NodeContainer::WHIL:
            return a.whil() == b.whil();
            break;
        case NodeContainer::UNINITIALIZED:
            break;
        default:
            throw std::runtime_error("Dont know what to do here!");
    }
    return false;
}

bool operator!=(const NodeContainer& a, const NodeContainer& b) { return !(a == b); }

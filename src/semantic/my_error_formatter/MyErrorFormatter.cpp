//
// Created by chris on 24/10/21.
//

#include "MyErrorFormatter.h"

std::string MyErrorFormatter::format(const error::BadReturn& err) const {
    return "Error::BadReturn";
}

std::string MyErrorFormatter::context_string(TextPosition position) const {
    std::string msg = E_HLT(text_pos_to_string(this->__file__, position)) + E_FMT(": ");
    return msg;
}

std::string MyErrorFormatter::code_context_string(TextPosition position) const {
    // return this->code_lines.get_line(position.line);
    std::string str = "\n" + this->code_lines.get_line(position.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(position.column, ' ') + std::string(1, '^'));
    return str;
}

std::string MyErrorFormatter::format(const error::TypeMismatch& err) const {
    std::string out;
    out += this->context_string(err.start) + "Error::TypeMismatch: expected '" + err.expected->to_string() +
           "' but got '" + entity_to_string(*err.actual) + "'" + this->code_context_string(err.start);
    return out;
}

std::string MyErrorFormatter::format(const error::TypeclassNotFound& err) const {
    return "Error::TypeclassNotFound: " + err.name;
}

MyErrorFormatter::MyErrorFormatter(const std::string& __file__, const CodeLines& code_lines)
        : __file__(__file__), code_lines(code_lines) {
}

std::string MyErrorFormatter::format(const error::GenericError& err) const {
    return this->context_string(err.start) + "Error::GenericError: " + err.msg + this->code_context_string(err.start);
}

std::string MyErrorFormatter::format(const error::Redeclared& err) const {
    return this->context_string(err.node.start) + "Error::Redeclared: '" + err.node.identifier.str + "'" +
           this->code_context_string(err.node.identifier.start);
}

std::string MyErrorFormatter::format(const error::For& err) const {
    return this->context_string(err.position) + "Error::For: expected a 'List'" +
           this->code_context_string(err.position);
}

std::string MyErrorFormatter::format(const error::NoMember& err) const {
    return this->context_string(err.m.start) + "Error::NoMember: '" + err.m.s_child + "'" +
           this->code_context_string(err.m.start);
}

std::string MyErrorFormatter::format(const error::NoMemberSuggestions& err) const {
    std::string msg = this->context_string(err.m.start) + "Error::NoMember: '" + err.m.s_child + "' in class " +
                      err.clazz.class_name + "'" + this->code_context_string(err.m.start);
    msg += "\ncandidates are:\n";
    for (auto& m: err.clazz.attributes) {
        msg += m.first + " : " + m.second->to_string() + "\n";
    }
    for (auto& m: err.clazz.methods) {
        msg += m.first + " : " + m.second.base.func.const_function_ft.to_string() + "\n";
    }
    return msg;
}

std::string MyErrorFormatter::format(const error::ObjectNoSpecialMethod& err) const {
    return this->context_string(err.node.start) + "Error::ObjectNoSpecialMethod: '" + err.method_name + "'" +
           this->code_context_string(err.node.start);
}

std::string MyErrorFormatter::format(const error::ClassNoMethodForOp& err) const {
    return this->context_string(err.node.start) + "Error::ClassNoMethodForOp: static method '" + err.op +
           "' required for this operation" + this->code_context_string(err.node.start);
}

std::string MyErrorFormatter::format(const error::NotDeclared& err) const {
    return this->context_string(err.idn.start) + "Error::NotDeclared: '" + err.idn._id + "'" +
           this->code_context_string(err.idn.start);
}

std::string MyErrorFormatter::format(const error::FunctionCallNumArgs& err) const {
    return this->context_string(err.start) + "Error::FunctionCallNumArgs:" + this->code_context_string(err.start);
}

std::string MyErrorFormatter::format(const error::ClassNoMember& err) const {
    return this->context_string(err.position) + "Error::ClassNoMember: class'" + err.class_path.basname() +
           "' has no member '" + err.member_name + "'" + this->code_context_string(err.position);
}

std::string MyErrorFormatter::format(const error::GlobalRedeclared& err) const {
    return this->context_string(err.id.start) + "Error::GlobalRedeclared: '" + err.id.str + "'" +
           this->code_context_string(err.id.start);
}

std::string MyErrorFormatter::format(const error::ImportNotFound& err) const {
    std::string msg;
    if (err.base_unit->is_package()) {
        Package& p = err.base_unit->package();
        msg = this->context_string(err.path_part.start) + "Error::ImportError: package '" + p.name + "' ('" +
              p.abs_path + "') has no member " + err.path_part.str + this->code_context_string(err.path_part.start);
    } else {
        Module& p = err.base_unit->module();
        msg = this->context_string(err.path_part.start) + "Error::ImportError: module '" + p.name + "' ('" +
              p.abs_path + "') has no member " + err.path_part.str + this->code_context_string(err.path_part.start);
    }
    return msg;
}

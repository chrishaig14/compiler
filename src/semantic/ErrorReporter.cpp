#include "ErrorReporter.h"
#include "Checker.h"

std::string entity_to_string(Entity entity) {
    std::string out;
    switch (entity.type) {
        case E_TYPE::PACKAGE:
            return "package " + E_HLT(entity.package->name);
        case E_TYPE::MODULE:
            return "module " + E_HLT(entity.module->name);
        case E_TYPE::CLASS:
            return "class " + E_HLT(entity.clazz->class_name);
        case E_TYPE::VALUE:
            return E_HLT(entity.value->type->to_string());
        case E_TYPE::CONST_FUNCTION:
            return "function " + E_HLT(entity.const_function->path.as_vec().back());
        case E_TYPE::ERROR:
            return "ERROR";
        case E_TYPE::NOT_FOUND:
            return "NOT FOUND";
        case E_TYPE::ENUM:
            return "enum " + E_HLT(entity.enumm->enumm_name);
    }
    return out;
}

void ErrorReporter::fail(std::string msg, TextPosition pos) {
    this->failed = true;
    std::string out = this->context_string(pos) + msg + this->code_context_string(pos);
    std::cout << out << std::endl;
    if (FAIL_FIRST) {
        throw std::runtime_error("Error");
    }
}

void ErrorReporter::fail_ok(std::string pre_msg, std::string msg, TextPosition pos) {
    this->failed = true;
    std::string out = this->context_string(pos) + "\n" + pre_msg + "\n\n" + msg +
                      "\n\n=====================================================================\n";
    std::cout << out << std::endl;
    if (FAIL_FIRST) {
        throw std::runtime_error("Error");
    }
}

std::string substring(std::string s, TextPosition start, TextPosition end) {
    assert(start.line == end.line);
    return s.substr(start.column, end.column - start.column);
}


std::map<ErrorElement, fmt::text_style> styles;

void init_styles() {
    styles[ErrorElement::BinopLeft] = fmt::fg(fmt::terminal_color::green) | fmt::emphasis::bold;
    styles[ErrorElement::BinopOperator] = fmt::fg(fmt::terminal_color::red) | fmt::emphasis::bold;
    styles[ErrorElement::BinopRight] = fmt::fg(fmt::terminal_color::blue) | fmt::emphasis::bold;
}

void ErrorReporter::entity_no_member(std::string pre_msg, const std::string& member, TextPosition pos, Node& obj,
                                     TextPosition member_start, TextPosition member_end) {
    std::string code_s = this->code_lines.get_line(pos.line);
    std::string pre_s = substring(code_s, TextPosition{obj.start.line, 0}, obj.start);
    std::string left_s = substring(code_s, obj.start, obj.end);
    std::string middle_s = substring(code_s, obj.end, member_start);
    std::string right_s = substring(code_s, member_start, member_end);
    std::string post_s = substring(code_s, member_end, TextPosition{member_end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], middle_s) +
                      fmt::format(styles[ErrorElement::BinopRight], right_s) + post_s;
    this->fail_ok(pre_msg + E_FMT(" has no member ") + E_HLT("'" + member + "'"), msg, pos);
}


void ErrorReporter::object_no_member(const TypeNode& t, const MemberNode& m) {
    this->entity_no_member(E_FMT("Object of type ") + E_HLT(t.to_string()),
                           m.s_child,
                           m.dot_pos,
                           *m.parent,
                           m.child_token.start,
                           m.child_token.end_pos);
}


void ErrorReporter::class_no_member(const TypeNode& t, const std::string& member, TextPosition pos, Node& obj,
                                    TextPosition member_start, TextPosition member_end) {
    this->entity_no_member(E_FMT("Class ") + E_HLT(t.to_string()), member, pos, obj, member_start, member_end);
}

void ErrorReporter::module_no_member(Module* mod, const std::string& member, TextPosition pos, Node& obj,
                                     TextPosition member_start, TextPosition member_end) {
    std::string pre_msg = E_FMT("Module ") + E_HLT(mod->name);

    std::string code_s = this->code_lines.get_line(pos.line);
    std::string pre_s = substring(code_s, TextPosition{obj.start.line, 0}, obj.start);
    std::string left_s = substring(code_s, obj.start, obj.end);
    std::string middle_s = substring(code_s, obj.end, member_start);
    std::string right_s = substring(code_s, member_start, member_end);
    std::string post_s = substring(code_s, member_end, TextPosition{member_end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], middle_s) +
                      fmt::format(styles[ErrorElement::BinopRight], right_s) + post_s;

    msg += "\n\nPossible members are:  \n";
    for (auto m: mod->flirpins) {
        msg += "- " + fmt::format(fmt::emphasis::bold, m.first) + " : " + flirpintype_to_str(m.second.type) + "\n";
    }
    msg = msg.substr(0, msg.size() - 1);
    this->fail_ok(pre_msg + E_FMT(" has no member ") + E_HLT("'" + member + "'"), msg, pos);
}

void ErrorReporter::package_no_member(Package* pack, const std::string& member, TextPosition pos, Node& obj,
                                      TextPosition member_start, TextPosition member_end) {
    std::string pre_msg = E_FMT("Package ") + E_HLT(pack->name);

    std::string code_s = this->code_lines.get_line(pos.line);
    std::string pre_s = substring(code_s, TextPosition{obj.start.line, 0}, obj.start);
    std::string left_s = substring(code_s, obj.start, obj.end);
    std::string middle_s = substring(code_s, obj.end, member_start);
    std::string right_s = substring(code_s, member_start, member_end);
    std::string post_s = substring(code_s, member_end, TextPosition{member_end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], middle_s) +
                      fmt::format(styles[ErrorElement::BinopRight], right_s) + post_s;

    msg += "\n\nPossible modules/packages are:  \n";
    for (auto m: pack->units) {
        msg += "- " + fmt::format(fmt::emphasis::bold, m.first) + " : " +
               (m.second.type == U_TYPE::MODULE ? "module" : "package") + "\n";
    }
    msg = msg.substr(0, msg.size() - 1);
    this->fail_ok(pre_msg + E_FMT(" has no member ") + E_HLT("'" + member + "'"), msg, pos);
}

void ErrorReporter::bool_op(Entity left, Entity right, TextPosition pos) {
    std::string msg;
    msg = E_FMT("Cannot perform bool op between types ") + E_HLT(entity_to_string(left)) + E_FMT(" and ") +
          E_HLT(entity_to_string(right));
    this->fail(msg, pos);
}

void ErrorReporter::assignment(const TypeNode& expected, const TypeNode& actual, TextPosition pos, const Node& lvalue,
                               const Node& rvalue) {
    std::string pre_msg;
    pre_msg = E_FMT("Expected ") + E_HLT(expected.to_string()) + E_FMT("(alias for ") +
              E_HLT(expected.actual_to_string()) + E_FMT(")") + E_FMT(", got ") + E_HLT(actual.to_string()) +
              E_FMT(" (alias for ") + E_HLT(actual.actual_to_string()) + E_FMT(")");

    std::string code_s = this->code_lines.get_line(pos.line);
    std::string pre_s = substring(code_s, TextPosition{lvalue.start.line, 0}, lvalue.start);
    std::string left_s = substring(code_s, lvalue.start, lvalue.end);
    std::string middle_s = substring(code_s, lvalue.end, rvalue.start);
    std::string right_s = substring(code_s, rvalue.start, rvalue.end);
    std::string post_s = substring(code_s, rvalue.end, TextPosition{rvalue.end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], middle_s) +
                      fmt::format(styles[ErrorElement::BinopRight], right_s) + post_s;
    this->fail_ok(pre_msg, msg, pos);

    // this->fail(msg, pos);
}

void ErrorReporter::no_return(const TypeNode& t, TextPosition pos) {
    std::string msg;
    msg = E_FMT(" Expected to return ") + E_HLT(t.to_string()) + E_FMT(" but not returning anything");
    this->fail(msg, pos);
}

void ErrorReporter::function_doesnt_return_a_value(TextPosition pos, const TypeNode* expected_type) {
    std::string msg;
    msg = E_FMT(" Function doesn't return a value but its being used as an expression") +
          (expected_type != nullptr ? E_FMT(", expected ") + E_HLT(expected_type->to_string()) : "");
    this->fail(msg, pos);
}

void ErrorReporter::bad_return(TextPosition pos) {
    std::string msg;
    msg = E_HLT(text_pos_to_string(this->__file__, pos)) +
          E_FMT(" Returning a value from a function returning no value ");
    this->fail(msg, pos);
}

void ErrorReporter::_for(Entity t, TextPosition pos) {
    std::string msg;
    msg = E_FMT(" Expected") + E_HLT(" List[t] ") + E_FMT("in loop, but got ") + E_HLT(entity_to_string(t));
    this->fail(msg, pos);
}

void ErrorReporter::call_bad_num_args() {
    std::string msg = "Function call with wrong number of arguments!";
    this->fail(msg, TextPosition());
}

void ErrorReporter::class_no_method(const std::string& class_name, const std::string method_name, TextPosition pos) {
    std::string msg;
    msg = E_FMT("Class ") + E_HLT(class_name) + E_FMT(" has no method ") + E_HLT(method_name);
    this->fail(msg, pos);
}

void ErrorReporter::redeclared(const std::string& name, const DeclarationNode& node) {
    std::string pre_msg = E_FMT("Variable ") + E_HLT(name) + E_FMT(" already declared ");
    this->fail_ok(pre_msg, highlight_one(node), node.start);
}

void ErrorReporter::variable_not_declared(const std::string& name, TextPosition pos) {
    std::string msg;
    msg = E_FMT("Variable ") + E_HLT("'" + name + "'") + E_FMT(" not declared");
    this->fail(msg, pos);
}

void ErrorReporter::error_type_mismatch(const TypeNode& expected, const Node& value_node, Entity actual) {
    std::string as;
    if (actual.type == E_TYPE::VALUE) {
        as = actual.value->type->to_string();
    } else {
        as = entity_to_string(actual);
    }
    std::string pre_msg = "Expected " + E_HLT(expected.to_string()) + ", got " + E_HLT(as);
    std::string msg = highlight_one(value_node);
    this->fail_ok(pre_msg, msg, value_node.start);
}

void ErrorReporter::unused_return_value(TextPosition pos) {
    std::string pre_msg = E_FMT("Unused return value of function call");
    std::string msg = this->code_lines.get_line(pos.line);
    this->fail_ok(pre_msg, msg, pos);
}

void ErrorReporter::function_call_num_args(FunctionType& ft, TextPosition pos) {
    std::string msg;
    msg = E_FMT("Calling function of type ") + E_HLT(ft.to_string()) + E_FMT(" with wrong number of arguments");
    this->fail(msg, pos);
}

std::string ErrorReporter::highlight_one(const Node& f) {
    std::string code_s = this->code_lines.get_line(f.start.line);
    std::string pre_s = substring(code_s, TextPosition{f.start.line, 0}, f.start);
    std::string node_s = substring(code_s, f.start, f.end);
    std::string post_s = substring(code_s, f.end, TextPosition{f.end.line, code_s.size()});
    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopOperator], node_s) + post_s;
    return msg;
}

std::string ErrorReporter::highlight_two(ErrorElement fe, const Node& f, ErrorElement se, const Node& s) {
    std::string code_s = this->code_lines.get_line(f.start.line);
    std::string pre_s = substring(code_s, TextPosition{f.start.line, 0}, f.start);
    std::string left_s = substring(code_s, f.start, f.end);
    std::string middle_s = substring(code_s, f.end, s.start);
    std::string right_s = substring(code_s, s.start, s.end);
    std::string post_s = substring(code_s, s.end, TextPosition{s.end.line, code_s.size()});
    std::string msg = pre_s + fmt::format(styles[fe], left_s) + middle_s + fmt::format(styles[se], right_s) + post_s;
    return msg;
}

void ErrorReporter::call_not_a_function(const CallNode& node) {
    std::string pre_msg;
    pre_msg = E_FMT(" Calling something that's not a function");

    std::string msg = this->highlight_two(ErrorElement::BinopLeft,
                                          *node.function,
                                          ErrorElement::BinopRight,
                                          IdNode("", node.function->end, node.end));
    this->fail_ok(pre_msg, msg, node.start);
}

void ErrorReporter::list_literal(const TypeNode& lt, const TypeNode& et, TextPosition pos, const Node& ell) {
    std::string pre_msg;
    pre_msg = E_FMT("List literal with element of wrong type, expected ") + E_HLT(lt.to_string()) + E_FMT(" got ") +
              E_HLT(et.to_string());
    std::string msg = this->highlight_one(ell);
    this->fail_ok(pre_msg, msg, pos);
}

void ErrorReporter::function_return_last_stmt(const std::string& function_name, const TypeNode& et, TextPosition pos) {
    std::string msg;
    msg = E_FMT("Error in function ") + E_HLT(function_name) +
          E_FMT(": The last statement must be a return <EXPRESSION> of type ") + E_HLT(et.to_string());
    this->fail(msg, pos);
}

void ErrorReporter::partial_wrong_num_args(TextPosition pos) {
    std::string msg;
    msg = E_FMT("wrong number of arguments for partial function");
    this->fail(msg, pos);
}

std::string ErrorReporter::context_string(TextPosition position) {
    std::string msg = E_HLT(text_pos_to_string(this->__file__, position)) + E_FMT(" In function ") +
                      E_HLT((this->current_class == "" ? "" : this->current_class + ".") + this->current_function) +
                      E_FMT(": ");
    return msg;
}

std::string ErrorReporter::code_context_string(TextPosition position) {
    // return this->code_lines.get_line(position.line);
    std::string str = "\n" + this->code_lines.get_line(position.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(position.column, ' ') + std::string(1, '^'));
    return str;
}

std::string ErrorReporter::code_error_string(TextPosition start, TextPosition end) {
    int length = end.column - start.column + 1;
    std::string str = "\n" + this->code_lines.get_line(start.line) + "\n";
    str += fmt::format(fmt::fg(fmt::color::orange_red), std::string(start.column, ' ') + std::string(length, '^'));
    return str;
}

void ErrorReporter::class_not_generic(const std::string& cls, TextPosition pos) {
    std::string msg;
    msg = E_FMT("Class ") + E_HLT(cls) + E_FMT(" is not generic");
    this->fail(msg, pos);
}

ErrorReporter::ErrorReporter() {
    this->failed = false;
    init_styles();
}

void ErrorReporter::expected_expression(Entity entity, const Node& pos) {
    std::string pre_msg;
    pre_msg = E_FMT("Expected expression, got ") + E_HLT(entity_to_string(entity));
    std::string msg = this->highlight_one(pos);
    this->fail_ok(pre_msg, msg, pos.start);
}

void ErrorReporter::class_no_method_for_op(std::string class_name, std::string method_name, const Node& node) {
    std::string pre_msg = E_FMT("Class ") + E_HLT(class_name) + " does not define " +
                          E_HLT("static fun " + method_name + "(" + class_name + ", " + class_name + ") -> " +
                                class_name) + " needed for this operation";
    std::string msg = this->highlight_one(node);
    this->fail_ok(pre_msg, msg, node.start);
}

void ErrorReporter::enum_no_value(std::string enum_name, std::string value, MemberNode& node, Enum* enumm) {
    TextPosition member_start = add_one_col(node.dot_pos);
    TextPosition member_end = node.end;
    Node& obj = *node.parent;
    std::string code_s = this->code_lines.get_line(node.start.line);
    std::string pre_s = substring(code_s, TextPosition{obj.start.line, 0}, obj.start);
    std::string left_s = substring(code_s, obj.start, obj.end);
    std::string middle_s = substring(code_s, obj.end, member_start);
    std::string right_s = substring(code_s, member_start, member_end);
    std::string post_s = substring(code_s, member_end, TextPosition{member_end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], middle_s) +
                      fmt::format(styles[ErrorElement::BinopRight], right_s) + post_s;
    std::string pre_msg = "Enum " + enum_name + E_FMT(" has no value ") + E_HLT("'" + value + "'");
    msg += "\n\nPossible values are: \n";
    for (auto v: enumm->values) {
        msg += "- " + fmt::format(fmt::emphasis::bold, v) + "\n";
    }
    msg = msg.substr(0, msg.size() - 1);
    this->fail_ok(pre_msg, msg, node.start);

}

void ErrorReporter::object_no_member_with_suggestions(const TypeNode& t, const std::string& member, TextPosition pos,
                                                      Node& obj, TextPosition member_start, TextPosition member_end,
                                                      Class* clazz) {
    std::string pre_msg = E_FMT("Object of type ") + E_HLT(t.to_string());

    std::string code_s = this->code_lines.get_line(pos.line);
    std::string pre_s = substring(code_s, TextPosition{obj.start.line, 0}, obj.start);
    std::string left_s = substring(code_s, obj.start, obj.end);
    std::string middle_s = substring(code_s, obj.end, member_start);
    std::string right_s = substring(code_s, member_start, member_end);
    std::string post_s = substring(code_s, member_end, TextPosition{member_end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], middle_s) +
                      fmt::format(styles[ErrorElement::BinopRight], right_s) + post_s;

    msg += "\n\nPossible members are:  \n";
    for (auto m: clazz->members) {
        msg += "- " + fmt::format(fmt::emphasis::bold, m.first) + " : " + m.second->to_string() + "\n";
    }
    for (auto m: clazz->methods) {
        msg += "- " + fmt::format(fmt::emphasis::bold, m.first) + " : " + m.second->ft->to_string() + "\n";
    }
    msg = msg.substr(0, msg.size() - 1);
    this->fail_ok(pre_msg + E_FMT(" has no member ") + E_HLT("'" + member + "'"), msg, pos);
}

void ErrorReporter::object_no_special_method(const TypeNode& type, const char* method_name, const SubscriptNode& node) {
    std::string pre_msg =
            E_FMT("Object of type ") + E_HLT(type.to_string()) + " does not implement method " + E_HLT(method_name);

    std::string code_s = this->code_lines.get_line(node.start.line);
    std::string pre_s = substring(code_s, TextPosition{node.start.line, 0}, node.start);
    std::string left_s = substring(code_s, node.parent->start, node.parent->end);
    std::string right_s = substring(code_s, node.parent->end, node.end);
    std::string post_s = substring(code_s, node.end, TextPosition{node.end.line, code_s.size()});

    std::string msg = pre_s + fmt::format(styles[ErrorElement::BinopLeft], left_s) +
                      fmt::format(styles[ErrorElement::BinopOperator], right_s) + post_s;
    this->fail_ok(pre_msg, msg, node.start);
}

void ErrorReporter::cant_assign(const Node& node) {
    std::string pre_msg = E_FMT("Can't assign");
    std::string msg = this->highlight_one(node);
    this->fail_ok(pre_msg, msg, node.start);
}

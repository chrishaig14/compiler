//
// Created by chris on 31/8/21.
//

#include "PythonTranspiler.h"
#include "../simple_nodes/common/include/Throw.h"

PythonOutputCode::PythonOutputCode(const std::string& pre_code, const std::string& code)
        : pre_code(pre_code), code(code) {
}

//
// Created by chris on 4/4/21.
//



PythonOutputCode PythonTranspiler::transpile_declaration(const sem::Declaration& node) {
    PythonOutputCode exp_out = this->dispatch_expression(node.expression);
    std::string code = exp_out.pre_code.empty() ? "" : exp_out.pre_code + "\n";
    code += this->indentation() + node.identifier + " = " + exp_out.code + "\n";
    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_assignment(const sem::Assignment& node) {
    std::string code;
    PythonOutputCode lvalue = this->dispatch_expression(*node.lvalue);
    std::string pre_code;
    if (not lvalue.pre_code.empty()) {
        pre_code = lvalue.pre_code;
    }
    PythonOutputCode rvalue = this->dispatch_expression(*node.rvalue);
    if (not rvalue.pre_code.empty()) {
        pre_code += rvalue.pre_code;
    }
    code += lvalue.code + SPACE + ASSIGN + SPACE + rvalue.code + NEWLINE;
    return PythonOutputCode(pre_code, code);
}

PythonOutputCode PythonTranspiler::transpile_return(const sem::Return& node) {
    if (node.expression == nullptr) {
        return PythonOutputCode("", this->indentation() + "return None");
    }
    PythonOutputCode exp = this->dispatch_expression(*node.expression);
    std::string pre_code = exp.pre_code;
    std::string code = this->indentation() + "return " + exp.code + "\n";
    return PythonOutputCode(pre_code, code);
}

// PythonOutputCode PythonTranspiler::transpile_throw(const sem::Throw& node) {
//     PythonOutputCode exp = this->dispatch(*node.expression);
//     std::string out = exp.pre_code;
//     out += RETURN_VAR + SPACE + ASSIGN + SPACE + "set_tag(" + LPAREN + SPACE + exp.code + RPAREN + ",EXCEPTION_TAG)" +
//            SEMIC + NEWLINE;
//     out += RETURN + SPACE + RETURN_VAR + SEMIC + NEWLINE;
//     return PythonOutputCode("", out);
// }

std::string path_to_id(std::string p) {
    std::string out;
    for (char c : p) {
        if (c == '.') {
            out += "_D_";
        } else {
            out += std::string(1, c);
        }
    }
    return out;
}

PythonOutputCode PythonTranspiler::transpile_id(const sem::Id& node) {
    // if (node.identifier == "") {
    //     throw std::runtime_error("Error: tranpiling empty idnode!");
    // }
    std::string out;
    // if (node.identifier == "this") {
    //     node.identifier = "this_obj";
    // }
    out += (node.identifier);
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_function(const sem::FunctionDef& node) {
    std::string f_source = "def ";
    std::string parameters;

    // for (auto& param : node.params) {
    //     if (param == "this") {
    //         param = "this_obj";
    //     }
    // }
    if (this->add_self) {
        parameters += "self, ";
    }
    for (const auto& pn: node.params) {
        parameters += pn + COMMA + SPACE;
    }
    parameters = parameters.substr(0, parameters.size() - 2);
    f_source += (node.identifier) + LPAREN + parameters + RPAREN + ":" + NEWLINE;
    // this->source += f_source;
    f_source += indent_paragraph(this->transpile_block(*node.body).code, 4);
    return PythonOutputCode("", f_source);
}

PythonOutputCode PythonTranspiler::transpile_block(const sem::Block& node) {
    std::string pre_code;
    std::string code;
    this->indent();
    for (auto& n: node.nodes) {
        PythonOutputCode statement_out = this->dispatch_common(*n);
        // if (n->type == sem::CommonType::CONST_FUNCTION_CALL || n->type == sem::CommonType::OBJECT_METHOD_CALL) {
        //     code += statement_out.pre_code + "\n";
        //     code += this->indentation() + statement_out.code + "\n";
        // } else {
        //     code += statement_out.code;
        // }
    }
    if (code.back() == '\n') {
        code = code.substr(0, code.size() - 1);
    }
    this->unindent();
    return PythonOutputCode("", code);
}

void PythonTranspiler::transpile_program(const sem::Block& node) {
    for (auto& n: node.nodes) {
        this->dispatch_top(*n);
    }
}

PythonOutputCode PythonTranspiler::transpile_integer(const sem::Integer& node) {
    return PythonOutputCode("", "libcore.libcore.Integer(" + node.str + ")");
}

PythonOutputCode PythonTranspiler::transpile_call(const sem::Call& node) {
    std::string pre_code;
    VectorOfStrings arg_names;
    std::string fofo;
    for (auto& arg: node.arguments) {
        std::string afofo;
        PythonOutputCode arg_code = this->dispatch_expression(*arg);
        if (arg->type == sem::ExpType::CALL) {
            afofo = "arg_" + std::to_string(rand());
            arg_names.push_back(afofo);
            pre_code += arg_code.pre_code;
            pre_code += TOBJECT + SPACE + afofo + SPACE + ASSIGN + SPACE + arg_code.code + SEMIC + NEWLINE;
        } else {
            afofo = arg_code.code;
        }
        fofo += afofo + COMMA + SPACE;
    }
    // std::cout << "PRE CODE:" << std::endl;
    // std::cout << pre_code << std::endl;

    std::string post_code;
    PythonOutputCode func = this->dispatch_expression(*node.function);
    pre_code += func.pre_code;
    post_code += "CALL" + std::to_string(node.arguments.size()) + "(" + func.code;
    post_code += COMMA + SPACE;
    post_code += fofo;
    post_code = post_code.substr(0, post_code.size() - 2);
    post_code += ")";
    // std::cout << "POST CODE:" << std::endl;
    // std::cout << post_code << std::endl;

    return PythonOutputCode(pre_code, post_code);

    // std::string out;
    // std::string arguments;
    // for (auto* arg: node.arguments) {
    //     std::string arg_s = this->dispatch(arg);
    //     arguments += arg_s + COMMA + SPACE;
    // }
    // arguments = arguments.substr(0, arguments.size() - 2);
    // out += "CALL" + std::to_string(node.arguments.size()) + "(" + this->dispatch(node.function);
    // if (!arguments.empty()) {
    //     out += +", " + arguments;
    // }
    // out += ")";
    // return out;
}

PythonOutputCode PythonTranspiler::transpile_string(const sem::String& node) {
    return PythonOutputCode("", "libcore.libcore.String" + LPAREN + QUOTE + node.s + QUOTE + RPAREN);
}

PythonOutputCode PythonTranspiler::transpile_boolean(const sem::Bool& node) {
    return PythonOutputCode("", node.v ? "True" : "False");
}

PythonOutputCode PythonTranspiler::transpile_float(const sem::Float& node) {
    return PythonOutputCode("", "MAKE_FLOAT(" + node.str + ")");
}

PythonOutputCode PythonTranspiler::transpile_new(const sem::NewObject& node) {
    std::string out;
    std::string class_id = (node.class_name);
    out += "NEW(" + class_id + COMMA + SPACE;
    if (class_id == "core_D_core_D_List") {
        out += "-------{";
    }
    for (auto& m: node.args) {
        if (m != nullptr) {

            PythonOutputCode arg_code = this->dispatch_expression(*m);
            out += arg_code.pre_code;
            out += arg_code.code + COMMA + SPACE;
        } else {
            out += "nullptr" + COMMA + SPACE;
        }
    }
    if (!node.args.empty()) {
        out = out.substr(0, out.size() - 2);
    }
    if (class_id == "core_D_core_D_List") {
        out += "}";
    }
    out += RPAREN;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_class(const sem::KlassDef& node) {
    std::string class_name = node.identifier;
    std::string code;
    code += "class " + class_name + ":\n";
    std::string def = "def __init__(self, ";
    for (const auto& m: node.members) {
        def += m + ", ";
    }
    def += "):\n";
    std::string block;
    for (const auto& m: node.members) {
        block += "self." + m + " = " + m + "\n";
    }
    block = indent_paragraph(block, 4);

    def += block;
    code += indent_paragraph(def, 4) + "\n";

    for (auto& m: node.methods) {
        this->add_self = true;
        PythonOutputCode fcode = this->transpile_function(*m);
        this->add_self = false;
        code += indent_paragraph(fcode.code, 4) + "\n";
        // std::cout << fcode.code << std::endl;
    }

    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_object_member(const sem::ObjectMember& node) {
    std::string out;
    PythonOutputCode object = this->dispatch_expression(*node.object);
    out += object.pre_code;
    out += "CAST" + LPAREN + object.code + COMMA + SPACE + (node.class_path.as_str()) + RPAREN + "->" +
           node.member_name;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_while(const sem::While& node) {
    // std::string out;
    // PythonOutputCode cond_out = this->dispatch(*node.condition);
    // PythonOutputCode body_out = this->transpile_block(*node.body);
    // std::string condition_name = "cond_" + std::to_string(rand());
    // out += cond_out.pre_code;
    // out += TOBJECT + SPACE + condition_name + SPACE + ASSIGN + SPACE + cond_out.code + SEMIC + NEWLINE;
    // out += "while" + SPACE + LPAREN + "GET_BOOL" + LPAREN + condition_name + RPAREN + RPAREN + SPACE + LCURLY +
    //        body_out.code + condition_name + SPACE + ASSIGN + SPACE + cond_out.code + SEMIC + NEWLINE + RCURLY;
    // return PythonOutputCode("", out);
    //
    std::string pre;
    std::string out;
    PythonOutputCode cond = this->dispatch_expression(*node.condition);
    std::string condition_name = "cond_" + std::to_string(rand());
    PythonOutputCode thenc = this->transpile_block(*node.body);
    if (cond.pre_code != "") {
        out += cond.pre_code;
        if (node.condition->type == sem::ExpType::CALL) {
            out += TOBJECT + SPACE + condition_name + SPACE + ASSIGN + cond.code + SEMIC + NEWLINE;
            out += "while" + SPACE + LPAREN + condition_name + RPAREN + ":" + NEWLINE + "    " + thenc.code;
        }
    } else {
        out += "while" + SPACE + LPAREN + cond.code + RPAREN + ":" + NEWLINE + "    " + thenc.code;
    }
    return PythonOutputCode("", out);

}

PythonOutputCode PythonTranspiler::transpile_list(const sem::List& node) {
    std::string out;
    out = "NEW(XList,{";
    for (auto& e: node.elements) {
        PythonOutputCode el = this->dispatch_expression(*e);
        out += el.pre_code;
        out += el.code + ", ";
    }
    if (!node.elements.empty()) {
        out = out.substr(0, out.size() - 2);
    }
    out += "})";
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_dict(const sem::Dict& node) {
    std::string out;
    out = "NEW(XDict,{";
    if (node.items.size() == 1) {
        auto& e = node.items[0];
        PythonOutputCode key = this->dispatch_expression(*e.first);
        PythonOutputCode value = this->dispatch_expression(*e.second);
        out += key.pre_code;
        out += value.pre_code;
        out += "std::make_pair(" + key.code + ", " + value.code + ")" + SPACE;
    }
    if (node.items.size() > 1) {
        for (auto& e: node.items) {
            PythonOutputCode key = this->dispatch_expression(*e.first);
            PythonOutputCode value = this->dispatch_expression(*e.second);
            out += key.pre_code;
            out += value.pre_code;
            out += "{" + key.code + ", " + value.code + "}" + COMMA + SPACE;
        }
        out = out.substr(0, out.size() - 2);
    }
    out += "})";
    return PythonOutputCode("", out);
}


PythonOutputCode PythonTranspiler::transpile_if(const sem::If& node) {
    std::string pre_code;
    std::string code;
    PythonOutputCode cond = this->dispatch_expression(node.condition);
    PythonOutputCode thenc = this->transpile_block(node.then);
    code += cond.pre_code.empty() ? "" : cond.pre_code + "\n";
    code += this->indentation() + "condition = " + cond.code + "\n";
    code += this->indentation() + "if" + SPACE + "condition:" + NEWLINE + indent_paragraph(thenc.code, 4);

    return PythonOutputCode(pre_code, code);
}

PythonOutputCode PythonTranspiler::transpile_break(const sem::Break& node) {
    std::string out;
    for (auto reachable: node.reachables) {
        if (reachable == "this") {
            reachable = "this_obj";
        }
        out += GCOUTOFSCOPE + LPAREN + reachable + RPAREN + SEMIC + NEWLINE;
    }
    out += "break" + SEMIC + NEWLINE;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_continue(const sem::Continue& node) {
    std::string out;
    for (auto reachable: node.reachables) {
        if (reachable == "this") {
            reachable = "this_obj";
        }
        out += GCOUTOFSCOPE + LPAREN + reachable + RPAREN + SEMIC + NEWLINE;
    }
    out += "continue" + SEMIC + NEWLINE;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_match(const sem::Match& node) {
    std::string out;
    PythonOutputCode exp = this->dispatch_expression(*node.exp);
    out += exp.pre_code;
    out += exp.code;
    out += "switch" + SPACE + LPAREN + "GET_INT(CAST(" + node.varname + ",core_D_core_D_Union" + RPAREN + "->type" +
           RPAREN + RPAREN + SPACE + LCURLY;
    for (auto c: node.cases) {
        PythonOutputCode mc = this->transpile_block(*c.second);
        out += "case" + SPACE + "" + std::to_string(c.first) + "" + SPACE + ":" + SPACE + LCURLY + mc.code + "break" +
               SEMIC + RCURLY;
    }
    out += RCURLY;
    return PythonOutputCode("", out);
}

void PythonTranspiler::transpile_enum(const sem::EnumDef& node) {
    std::string out;
    std::string enum_name = (node.id);
    // out += "enum class" + SPACE + enum_name + SPACE + " {\n";
    // out += "";
    for (const auto& m: node.values) {
        out += "extern" + SPACE + TOBJECT + enum_name + "_" + m + SEMIC + NEWLINE;
    }
    this->header += out;
    int k = 0;
    out = "";
    for (const auto& m: node.values) {
        out += TOBJECT + enum_name + "_" + m + ASSIGN + "MAKE_INT(" + std::to_string(k) + ")" + SEMIC + NEWLINE;
        k++;
    }
    this->source += out;

    // out += RCURLY + SEMIC + NEWLINE;
    out = "";
    std::string eq_name = enum_name + "_D___eq__";
    out += "extern" + SPACE + TOBJECT + eq_name + SEMIC;
    this->static_initializations += eq_name + SPACE + ASSIGN + SPACE + "core_D_core_D_Integer_D___eq__" + SEMIC;

    this->source += TOBJECT + SPACE + eq_name + ASSIGN + "nullptr" + SEMIC + NEWLINE;

    std::string ne_name = enum_name + "_D___ne__";
    out += "extern" + SPACE + TOBJECT + ne_name + SEMIC;
    this->static_initializations += ne_name + SPACE + ASSIGN + SPACE + "core_D_core_D_Integer_D___ne__" + SEMIC;

    this->source += TOBJECT + SPACE + ne_name + ASSIGN + "nullptr" + SEMIC + NEWLINE;
    this->header += out;
}

PythonOutputCode PythonTranspiler::transpile_enum_member(const sem::EnumMember& node) {
    std::string out;
    out += (node.enum_name) + "_" + node.value;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_ternary(const sem::Ternary& node) {
    std::string out;
    PythonOutputCode tern = this->dispatch_expression(*node.ext);
    out = tern.pre_code;
    PythonOutputCode truec = this->dispatch_expression(*node.true_case);
    out += truec.pre_code;
    PythonOutputCode falsec = this->dispatch_expression(*node.false_case);
    out += falsec.pre_code;
    out += LPAREN + "(it=" + tern.code + ")!=nullptr? " + truec.code + SPACE + ":" + SPACE + falsec.code + RPAREN;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_none(const sem::None& node) {
    return PythonOutputCode("", "nullptr");
}

PythonOutputCode PythonTranspiler::transpile_try_catch(const sem::TryCatch& node) {
    this->in_try_catch = true;
    PythonOutputCode body_out = this->transpile_block(*node.body);
    this->in_try_catch = false;
    std::string out = "TaggedObject* thrown_exception = nullptr;\n" + body_out.code;
    out += "if (thrown_exception!=nullptr){\n";
    for (size_t i = 0; i < node.catches_bodies.size(); i++) {
        PythonOutputCode catch_out = this->transpile_block((const sem::Block&) *node.catches_bodies[i]);
        out += "if (UNTAG(thrown_exception)->class_name==\"" + (node.e_names_types[i].second) + "\"){TaggedObject*" +
               node.e_names_types[i].first + "=thrown_exception;\n" + catch_out.code + "} else ";
    }
    out += RETURN + SPACE + "CALL1(test_D_bootstrap_D_Exception_D___init__,test_D_bootstrap_D_Exception_D___init__)" +
           SEMIC + NEWLINE;
    // out = out.substr(0, out.size() - 5);
    out += "\n}";
    return PythonOutputCode("", out);
}

std::string PythonTranspiler::transpile_module(const sem::Block& block) {
    std::string code;
    for (auto& n: block.nodes) {
        PythonOutputCode definition_output = this->dispatch_top(*n);
        code += definition_output.code + "\n";
    }
    return code;
}

PythonOutputCode PythonTranspiler::dispatch_top(const sem::Common& node) {
    switch (node.type) {
        case sem::CommonType::FUNCTION:
            return this->transpile_function((const sem::FunctionDef&) (node));
            break;
            // case SNodeType::ENUM:
            //     return this->transpile_enum((const sem::EnumDef&) node);
            //     break;
        case sem::CommonType::CLASS:
            return this->transpile_class((const sem::KlassDef&) node);
            break;
        default:
            throw std::runtime_error("Don't know what to do with this SNode!");
    }
}

PythonOutputCode PythonTranspiler::dispatch_common(const sem::Common& node) {
    switch (node.type) {
        case sem::CommonType::BLOCK:
            return this->transpile_block((const sem::Block&) node);
            // case sem::CommonType::ENUM_MEMBER:
            //     return this->transpile_enum_member((const sem::EnumMember&) node);
            // case sem::CommonType::BOOLEAN:
            //     return this->transpile_boolean((const sem::Bool&) node);
        case sem::CommonType::MATCH:
            return this->transpile_match((const sem::Match&) node);
        case sem::CommonType::IF:
            return this->transpile_if((const sem::If&) node);
        case sem::CommonType::BREAK:
            return this->transpile_break((const sem::Break&) node);
        case sem::CommonType::CONTINUE:
            return this->transpile_continue((const sem::Continue&) node);
            // case sem::CommonType::ID:
            //     return this->transpile_id((const sem::Id&) node);
            // case sem::CommonType::STRING:
            //     return this->transpile_string((const sem::String&) node);
        case sem::CommonType::CALL:
            return this->transpile_call((const sem::Call&) node);
            // case sem::CommonType::NEW:
            //     return this->transpile_new((const sem::NewObject&) node);
            // case sem::CommonType::DICT:
            //     return this->transpile_dict((const sem::Dict&) node);
            // case sem::CommonType::LIST:
            //     return this->transpile_list((const sem::List&) node);
            // case sem::CommonType::OBJECT_MEMBER:
            //     return this->transpile_object_member((const sem::ObjectMember&) node);
        case sem::CommonType::DECLARATION:
            return this->transpile_declaration((const sem::Declaration&) node);
        case sem::CommonType::WHILE:
            return this->transpile_while((const sem::While&) node);
        case sem::CommonType::ASSIGNMENT:
            return this->transpile_assignment((const sem::Assignment&) node);
        case sem::CommonType::RETURN:
            return this->transpile_return((const sem::Return&) node);
        case sem::CommonType::TRY_CATCH:
            return this->transpile_try_catch((const sem::TryCatch&) node);
            // case sem::SNodeType::THROW:
            //     return this->transpile_throw((const sem::Throw&) node);
            //     break;
            // case sem::CommonType::FLOAT:
            //     return this->transpile_float((const sem::Float&) node);
            // case sem::CommonType::NONE:
            //     return this->transpile_none((const sem::None&) node);
            // case sem::CommonType::TERNARY:
            //     return this->transpile_ternary((const sem::Ternary&) node);
            // case sem::CommonType::INTEGER:
            //     return this->transpile_integer((const sem::Integer&) node);
            //     break;
        default:
            throw std::runtime_error("Don't know what to do with this SNode!");
    }
}

PythonOutputCode PythonTranspiler::dispatch_expression(const sem::Exp& node) {
    switch (node.type) {
        case sem::ExpType::ENUM_MEMBER:
            return this->transpile_enum_member((const sem::EnumMember&) node);
        case sem::ExpType::BOOLEAN:
            return this->transpile_boolean((const sem::Bool&) node);
        case sem::ExpType::ID:
            return this->transpile_id((const sem::Id&) node);
        case sem::ExpType::STRING:
            return this->transpile_string((const sem::String&) node);
        case sem::ExpType::CALL:
            return this->transpile_call((const sem::Call&) node);
        case sem::ExpType::NEW:
            return this->transpile_new((const sem::NewObject&) node);
        case sem::ExpType::DICT:
            return this->transpile_dict((const sem::Dict&) node);
        case sem::ExpType::LIST:
            return this->transpile_list((const sem::List&) node);
        case sem::ExpType::OBJECT_MEMBER:
            return this->transpile_object_member((const sem::ObjectMember&) node);
        case sem::ExpType::FLOAT:
            return this->transpile_float((const sem::Float&) node);
        case sem::ExpType::NONE:
            return this->transpile_none((const sem::None&) node);
        case sem::ExpType::TERNARY:
            return this->transpile_ternary((const sem::Ternary&) node);
        case sem::ExpType::INTEGER:
            return this->transpile_integer((const sem::Integer&) node);
        case sem::ExpType::OBJECT_METHOD:
            return this->transpile_object_method((const sem::ObjectMethod&) node);
        case sem::ExpType::CONST_FUNCTION:
            return this->transpile_const_function((const sem::ConstFunction&) node);
        case sem::ExpType::OBJECT_CONSTRUCTOR:
            return this->transpile_object_constructor((const sem::ObjectConstructor&) node);
    }
    __builtin_unreachable();
}

size_t PythonTranspiler::next_arg_n() {
    return this->arg_n++;
}

void PythonTranspiler::indent() {
    this->indent_level += 4;
}

std::string PythonTranspiler::indentation() {
    return std::string(this->indent_level, ' ');
}

void PythonTranspiler::unindent() {
    this->indent_level -= 4;
}

PythonTranspiler::PythonTranspiler() {
    this->indent_level = 0;
    this->arg_n = 0;
}

PythonOutputCode PythonTranspiler::transpile_object_method(const sem::ObjectMethod& method) {
    return PythonOutputCode("", "");
}

PythonOutputCode PythonTranspiler::transpile_const_function(const sem::ConstFunction& function) {
    return PythonOutputCode("", "");
}

PythonOutputCode PythonTranspiler::transpile_object_constructor(const sem::ObjectConstructor& constructor) {
    return PythonOutputCode("", "");
}

std::string indent_paragraph(std::string s, size_t level) {

    std::string r = std::string(level, ' ');
    for (auto x: s) {
        r += x;
        if (x == '\n') {
            r += std::string(level, ' ');
        }
    }
    return r;
}

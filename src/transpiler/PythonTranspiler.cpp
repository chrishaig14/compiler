//
// Created by chris on 31/8/21.
//

#include "PythonTranspiler.h"
#include "PythonTranspiler.h"
#include "../simple_nodes/Throw.h"
#include "../simple_nodes/with_unique/ObjectMethodCall.h"
#include "../simple_nodes/with_unique/ConstFunctionCall.h"

PythonOutputCode::PythonOutputCode(const std::string& pre_code, const std::string& code)
        : pre_code(pre_code), code(code) {
}

//
// Created by chris on 4/4/21.
//



PythonOutputCode PythonTranspiler::transpile_declaration(const sem::Declaration& node) {
    PythonOutputCode exp_out = this->dispatch(*node.expression);
    std::string code = exp_out.pre_code.empty() ? "" : exp_out.pre_code + "\n";
    code += this->indentation() + node.identifier + " = " + exp_out.code + "\n";
    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_assignment(const sem::Assignment& node) {
    std::string code;
    PythonOutputCode lvalue = this->dispatch(*node.lvalue);
    std::string pre_code;
    if (not lvalue.pre_code.empty()) {
        pre_code = lvalue.pre_code;
    }
    PythonOutputCode rvalue = this->dispatch(*node.rvalue);
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
    PythonOutputCode exp = this->dispatch(*node.expression);
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

    for (const auto& pn: node.params) {
        parameters += pn + COMMA + SPACE;
    }
    parameters = parameters.substr(0, parameters.size() - 2);
    f_source += (node.identifier) + LPAREN + parameters + RPAREN + ":" + NEWLINE;
    // this->source += f_source;
    f_source += this->transpile_block(*node.body).code;
    return PythonOutputCode("", f_source);
}

PythonOutputCode PythonTranspiler::transpile_block(const sem::Block& node) {
    std::string pre_code;
    std::string code;
    this->indent();
    for (auto& n: node.nodes) {
        PythonOutputCode statement_out = this->dispatch(*n);
        if (n->type == SNodeType::CONST_FUNCTION_CALL || n->type == SNodeType::OBJECT_METHOD_CALL) {
            code += statement_out.pre_code + "\n";
            code += this->indentation() + statement_out.code + "\n";
        } else {
            code += statement_out.code;
        }
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
        PythonOutputCode arg_code = this->dispatch(*arg);
        if (arg->type == SNodeType::CALL) {
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
    PythonOutputCode func = this->dispatch(*node.function);
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

void PythonTranspiler::transpile_class(const sem::KlassDef& node) {
    std::string out;
    std::string class_name = (node.identifier);
    out += CLASS + SPACE + class_name + SPACE + ": public XObject {\n";
    out += "public: \n";
    for (const auto& m: node.members) {
        out += TOBJECT + SPACE + m + SEMIC + NEWLINE;
    }

    out += class_name + LPAREN;
    for (const auto& m: node.members) {
        out += TOBJECT + SPACE + m + COMMA + SPACE;
    }
    out = out.substr(0, out.size() - 2);
    out += RPAREN + SPACE + ":" + SPACE + "XObject" + LPAREN + QUOTE + class_name + QUOTE + RPAREN + SPACE + LCURLY +
           NEWLINE;
    for (const auto& m: node.members) {
        out += "this->" + m + " = " + m + SEMIC + NEWLINE;
    }
    out += RCURLY + NEWLINE;

    out += "~" + class_name + LPAREN + RPAREN + LCURLY + NEWLINE;
    out += "if (!GC::collecting){\n";
    for (const auto& m: node.members) {
        out += GCOUTOFSCOPE + LPAREN + "this->" + m + RPAREN + SEMIC + NEWLINE;
    }
    out += RCURLY;
    out += RCURLY + NEWLINE;

    out += "std::vector<XObject*> get_all_members() override {\n";
    out += "std::vector<XObject*> r;\n";
    for (const auto& m: node.members) {
        out += "if (has_tag(this->" + m + ",OBJECT_TAG)){r.push_back(UNTAG(this->" + m + "));}";
    }
    out += "return r;";
    out += RCURLY + NEWLINE;

    out += "TaggedObject* str() override {\n";
    out += "TaggedObject* r = MAKE_STRING(\"\");\n";
    out += "XString* _r = CAST(r, XString);\n";
    for (const auto& m: node.members) {
        out += "if (has_tag(this->" + m + ",OBJECT_TAG)){_r->s+=CAST(CAST(this->" + m +
               ", XObject)->str(),XString)->s;}";
    }
    out += "return r;";
    out += RCURLY + NEWLINE;

    out += RCURLY + SEMIC + NEWLINE;
    this->header += out;
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

            PythonOutputCode arg_code = this->dispatch(*m);
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

PythonOutputCode PythonTranspiler::transpile_object_member(const sem::ObjectMember& node) {
    std::string out;
    PythonOutputCode object = this->dispatch(*node.object);
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
    PythonOutputCode cond = this->dispatch(*node.condition);
    std::string condition_name = "cond_" + std::to_string(rand());
    PythonOutputCode thenc = this->transpile_block(*node.body);
    if (cond.pre_code != "") {
        out += cond.pre_code;
        if (node.condition->type == SNodeType::CALL) {
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
        PythonOutputCode el = this->dispatch(*e);
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
        PythonOutputCode key = this->dispatch(*e.first);
        PythonOutputCode value = this->dispatch(*e.second);
        out += key.pre_code;
        out += value.pre_code;
        out += "std::make_pair(" + key.code + ", " + value.code + ")" + SPACE;
    }
    if (node.items.size() > 1) {
        for (auto& e: node.items) {
            PythonOutputCode key = this->dispatch(*e.first);
            PythonOutputCode value = this->dispatch(*e.second);
            out += key.pre_code;
            out += value.pre_code;
            out += "{" + key.code + ", " + value.code + "}" + COMMA + SPACE;
        }
        out = out.substr(0, out.size() - 2);
    }
    out += "})";
    return PythonOutputCode("", out);
}


PythonOutputCode PythonTranspiler::transpile_if(const sem::IfSNode& node) {
    std::string pre_code;
    std::string code;
    PythonOutputCode cond = this->dispatch(*node.condition);
    PythonOutputCode thenc = this->transpile_block(*node.then);
    code += cond.pre_code.empty() ? "" : cond.pre_code + "\n";
    code += this->indentation() + "condition = " + cond.code + "\n";
    code += this->indentation() + "if" + SPACE + "condition:" + NEWLINE + thenc.code;

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
    PythonOutputCode exp = this->dispatch(*node.exp);
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
    PythonOutputCode tern = this->dispatch(*node.ext);
    out = tern.pre_code;
    PythonOutputCode truec = this->dispatch(*node.true_case);
    out += truec.pre_code;
    PythonOutputCode falsec = this->dispatch(*node.false_case);
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

PythonOutputCode PythonTranspiler::transpile_object_method_call(const sem::ObjectMethodCall& call) {
    PythonOutputCode object_code = this->dispatch(*call.object);
    std::string pre_code;
    std::string args_list;
    std::string call_id = std::to_string(rand());
    for (size_t i = 0; i < call.args.size(); i++) {
        PythonOutputCode arg_code = this->dispatch(*call.args[i]);
        if (not arg_code.pre_code.empty()) {
            pre_code += arg_code.pre_code.empty() ? "" : (arg_code.pre_code + "\n");
        }
        std::string arg_id = "arg" + call_id + "_" + std::to_string(i);
        args_list += arg_id + ", ";
        pre_code += arg_id + " = " + arg_code.code + "\n";
    }
    std::string object_id = "object_" + std::to_string(rand());
    pre_code =
            (object_code.pre_code.empty() ? "" : (object_code.pre_code + "\n")) + object_id + " = " + object_code.code +
            "\n" + pre_code;
    pre_code = pre_code.substr(0, pre_code.size() - 1);
    std::string global_function_name = call.class_path.as_str() + "." + call.method_name;
    std::string code = global_function_name + LPAREN + object_id + COMMA + SPACE + args_list;
    code = code.substr(0, code.size() - 2);
    code += RPAREN;
    return PythonOutputCode(pre_code, code);
}

PythonOutputCode PythonTranspiler::transpile_const_function_call(const sem::ConstFunctionCall& call) {
    std::string pre_code;
    std::string args_list;
    std::string call_id = std::to_string(rand());
    for (size_t i = 0; i < call.args.size(); i++) {
        PythonOutputCode arg_code = this->dispatch(*call.args[i]);
        if (not arg_code.pre_code.empty()) {
            pre_code += arg_code.pre_code.empty() ? "" : (arg_code.pre_code + "\n");
        }
        std::string arg_id = "arg" + call_id + "_" + std::to_string(i);
        pre_code += arg_id + " = " + arg_code.code + "\n";
        args_list += arg_id + ", ";
    }
    pre_code = indent_paragraph(pre_code.substr(0, pre_code.size() - 1), this->indent_level);
    std::string global_function_name = (call.path.as_str());
    std::string code = global_function_name + LPAREN + args_list;
    code = code.substr(0, code.size() - 2);
    code += RPAREN;
    return PythonOutputCode(pre_code, code);
}

std::string PythonTranspiler::transpile_module(const sem::Block& block) {
    std::string code;
    for (auto& n: block.nodes) {
        PythonOutputCode definition_output = this->dispatch_top(*n);
        code += definition_output.code + "\n";
    }
    return code;
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

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
    PythonOutputCode exp_out = this->dispatch_expression(node.expression, false);
    std::string code = exp_out.pre_code.empty() ? "" : exp_out.pre_code + "\n";
    code += node.identifier + " = " + exp_out.code;
    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_assignment(const sem::Assignment& node) {
    PythonOutputCode lvalue = this->dispatch_expression(*node.lvalue, false);
    PythonOutputCode rvalue = this->dispatch_expression(*node.rvalue, false);
    std::string code = pre_if_any(lvalue) + pre_if_any(rvalue);
    code += lvalue.code + SPACE + ASSIGN + SPACE + rvalue.code;
    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_return(const sem::Return& node) {
    if (node.expression == nullptr) {
        return PythonOutputCode("", "return None\n");
    }
    PythonOutputCode exp = this->dispatch_expression(*node.expression, false);
    std::string code = (exp.pre_code.empty() ? "" : exp.pre_code + "\n") + "rv = " + exp.code + "\n" + "return rv\n";
    return PythonOutputCode("", code);
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
    std::string id = node.identifier;
    if (id == "this" && this->add_self) {
        id = "self";
    }
    return PythonOutputCode("", id);
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
        code += statement_out.code + "\n";
    }
    if (code.back() == '\n') {
        code = code.substr(0, code.size() - 1);
    }
    this->unindent();
    return PythonOutputCode("", code);
}

void PythonTranspiler::transpile_program(const sem::Module& node) {
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
    std::string arg_list;
    PythonOutputCode func = this->dispatch_expression(*node.function, false);
    std::string fun_id = "function_to_call_" + std::to_string(this->next_arg_n());
    pre_code += (func.pre_code.empty() ? "" : func.pre_code + "\n") + fun_id + " = " + func.code + "\n";
    for (auto& arg: node.arguments) {
        PythonOutputCode arg_code = this->dispatch_expression(*arg, false);
        std::string arg_id = "arg_" + std::to_string(this->next_arg_n());
        pre_code += (arg_code.pre_code.empty() ? "" : arg_code.pre_code + "\n") + arg_id + " = " + arg_code.code + "\n";
        arg_list += arg_id + ", ";
    }
    std::string post_code = pre_code;
    post_code += fun_id + "(";
    post_code += arg_list.empty() ? "" : arg_list.substr(0, arg_list.size() - 2);
    post_code += ")";
    return PythonOutputCode("", post_code);
}

PythonOutputCode PythonTranspiler::transpile_string(const sem::String& node) {
    return PythonOutputCode("", "libcore.libcore.String" + LPAREN + QUOTE + node.s + QUOTE + RPAREN);
}

PythonOutputCode PythonTranspiler::transpile_boolean(const sem::Bool& node) {
    return PythonOutputCode("", std::string("libcore.libcore.Boolean(") + (node.v ? "True" : "False") + ")");
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

            PythonOutputCode arg_code = this->dispatch_expression(*m, false);
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
    PythonOutputCode object = this->dispatch_expression(*node.object, false);
    std::string obj_id = "obj_" + std::to_string(this->next_arg_n());
    std::string pre_code =
            (object.pre_code.empty() ? "" : object.pre_code + "\n") + obj_id + " = " + object.code + "\n";
    std::string code = obj_id + "." + node.member_name;
    return PythonOutputCode(pre_code, code);
}

PythonOutputCode PythonTranspiler::transpile_while(const sem::While& node) {
    PythonOutputCode cond = this->dispatch_expression(*node.condition, false);
    std::string cond_id = "condition_" + std::to_string(this->next_arg_n());
    PythonOutputCode thenc = this->transpile_block(*node.body);
    std::string out = pre_if_any(cond) + cond_id + " = " + cond.code + "\n";
    out += "while " + cond_id + ":\n";
    out += indent_paragraph(thenc.code, 4) + "\n";
    out += indent_paragraph(pre_if_any(cond) + cond_id + " = " + cond.code, 4);
    return PythonOutputCode("", out);

}

PythonOutputCode PythonTranspiler::transpile_list(const sem::List& node) {
    std::string pre_code;
    std::string elem_ids;
    for (auto& e: node.elements) {
        PythonOutputCode element_code = this->dispatch_expression(*e, false);
        std::string elem_id = "elem_" + std::to_string(this->next_arg_n());
        pre_code += pre_if_any(element_code) + elem_id + " = " + element_code.code + "\n";
        elem_ids += elem_id + ", ";
    }
    if (not elem_ids.empty()) {
        elem_ids = elem_ids.substr(0, elem_ids.size() - 2);
    }
    std::string code = "libcore.libcore.List([" + elem_ids + "])";
    return PythonOutputCode(pre_code, code);
}

PythonOutputCode PythonTranspiler::transpile_dict(const sem::Dict& node) {
    std::string out;
    out = "NEW(XDict,{";
    if (node.items.size() == 1) {
        auto& e = node.items[0];
        PythonOutputCode key = this->dispatch_expression(*e.first, false);
        PythonOutputCode value = this->dispatch_expression(*e.second, false);
        out += key.pre_code;
        out += value.pre_code;
        out += "std::make_pair(" + key.code + ", " + value.code + ")" + SPACE;
    }
    if (node.items.size() > 1) {
        for (auto& e: node.items) {
            PythonOutputCode key = this->dispatch_expression(*e.first, false);
            PythonOutputCode value = this->dispatch_expression(*e.second, false);
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
    std::string code;
    PythonOutputCode cond = this->dispatch_expression(node.condition, false);
    PythonOutputCode thenc = this->transpile_block(node.then);
    std::string cond_id = "condition_" + std::to_string(this->next_arg_n());
    code += cond.pre_code.empty() ? "" : cond.pre_code + "\n";
    code += cond_id + " = " + cond.code + "\n";
    code += "if" + SPACE + cond_id + ":" + NEWLINE + indent_paragraph(thenc.code, 4);

    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_break(const sem::Break& node) {
    return PythonOutputCode("", "");
}

PythonOutputCode PythonTranspiler::transpile_continue(const sem::Continue& node) {
    return PythonOutputCode("", "");
}

PythonOutputCode PythonTranspiler::transpile_match(const sem::Match& node) {
    std::string out;
    PythonOutputCode exp = this->dispatch_expression(*node.exp, false);
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

PythonOutputCode PythonTranspiler::transpile_enum(const sem::EnumDef& node) {
    return PythonOutputCode("", "");
}

PythonOutputCode PythonTranspiler::transpile_enum_member(const sem::EnumMember& node) {
    std::string out;
    out += (node.enum_name) + "_" + node.value;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_ternary(const sem::Ternary& node) {
    std::string out;
    PythonOutputCode tern = this->dispatch_expression(*node.ext, false);
    out = tern.pre_code;
    PythonOutputCode truec = this->dispatch_expression(*node.true_case, false);
    out += truec.pre_code;
    PythonOutputCode falsec = this->dispatch_expression(*node.false_case, false);
    out += falsec.pre_code;
    out += LPAREN + "(it=" + tern.code + ")!=nullptr? " + truec.code + SPACE + ":" + SPACE + falsec.code + RPAREN;
    return PythonOutputCode("", out);
}

PythonOutputCode PythonTranspiler::transpile_none(const sem::None& node) {
    return PythonOutputCode("", "nullptr");
}

PythonOutputCode PythonTranspiler::transpile_try_catch(const sem::TryCatch& node) {
    // this->in_try_catch = true;
    // PythonOutputCode body_out = this->transpile_block(*node.body);
    // this->in_try_catch = false;
    // std::string out = "TaggedObject* thrown_exception = nullptr;\n" + body_out.code;
    // out += "if (thrown_exception!=nullptr){\n";
    // for (size_t i = 0; i < node.catches_bodies.size(); i++) {
    //     PythonOutputCode catch_out = this->transpile_block((const sem::Block&) *node.catches_bodies[i]);
    //     out += "if (UNTAG(thrown_exception)->class_name==\"" + (node.e_names_types[i].second) + "\"){TaggedObject*" +
    //            node.e_names_types[i].first + "=thrown_exception;\n" + catch_out.code + "} else ";
    // }
    // out += RETURN + SPACE + "CALL1(test_D_bootstrap_D_Exception_D___init__,test_D_bootstrap_D_Exception_D___init__)" +
    //        SEMIC + NEWLINE;
    // // out = out.substr(0, out.size() - 5);
    // out += "\n}";
    // return PythonOutputCode("", out);
    return PythonOutputCode("", "");
}

std::string PythonTranspiler::transpile_module(const sem::Module& block, Path path) {
    this->module_path = path;
    std::string code;
    for (auto& n: block.nodes) {
        PythonOutputCode definition_output = this->dispatch_top(*n);
        code += definition_output.code + "\n";
    }
    return code;
}

PythonOutputCode PythonTranspiler::dispatch_top(const sem::Top& node) {
    switch (node.type) {
        case sem::TopType::FUNCTION:
            return this->transpile_function((const sem::FunctionDef&) (node));
        case sem::TopType::ENUM:
            return this->transpile_enum((const sem::EnumDef&) node);
        case sem::TopType::CLASS:
            return this->transpile_class((const sem::KlassDef&) node);
    }
    __builtin_unreachable();
}

PythonOutputCode PythonTranspiler::dispatch_common(const sem::Common& node) {
    switch (node.type) {
        case sem::CommonType::BLOCK:
            return this->transpile_block((const sem::Block&) node);
        case sem::CommonType::FOR:
            return this->transpile_for((const sem::For&) node);
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

PythonOutputCode PythonTranspiler::dispatch_expression(const sem::Exp& node, bool called_function) {
    switch (node.type) {
        case sem::ExpType::ENUM_MEMBER:
            return this->transpile_enum_member(static_cast<const sem::EnumMember&>(node));
        case sem::ExpType::BOOLEAN:
            return this->transpile_boolean(static_cast<const sem::Bool&>(node));
        case sem::ExpType::ID:
            return this->transpile_id(static_cast<const sem::Id&>(node));
        case sem::ExpType::STRING:
            return this->transpile_string(static_cast<const sem::String&>(node));
        case sem::ExpType::CALL:
            return this->transpile_call_exp(static_cast<const sem::CallExp&>(node));
        case sem::ExpType::NEW:
            return this->transpile_new(static_cast<const sem::NewObject&>(node));
        case sem::ExpType::DICT:
            return this->transpile_dict(static_cast<const sem::Dict&>(node));
        case sem::ExpType::LIST:
            return this->transpile_list(static_cast<const sem::List&>(node));
        case sem::ExpType::OBJECT_MEMBER:
            return this->transpile_object_member(static_cast<const sem::ObjectMember&>(node));
        case sem::ExpType::FLOAT:
            return this->transpile_float(static_cast<const sem::Float&>(node));
        case sem::ExpType::NONE:
            return this->transpile_none(static_cast<const sem::None&>(node));
        case sem::ExpType::TERNARY:
            return this->transpile_ternary(static_cast<const sem::Ternary&>(node));
        case sem::ExpType::INTEGER:
            return this->transpile_integer(static_cast<const sem::Integer&>(node));
        case sem::ExpType::OBJECT_METHOD:
            return this->transpile_object_method(static_cast<const sem::ObjectMethod&>(node), false);
        case sem::ExpType::CONST_FUNCTION:
            return this->transpile_const_function(static_cast<const sem::ConstFunction&>(node));
        case sem::ExpType::OBJECT_CONSTRUCTOR:
            return this->transpile_object_constructor(static_cast<const sem::ObjectConstructor&>(node));
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
    this->add_self = false;
}

PythonOutputCode PythonTranspiler::transpile_object_method(const sem::ObjectMethod& method, bool called_function) {
    std::string obj_id = "obj_" + std::to_string(this->next_arg_n());
    PythonOutputCode obj_code = this->dispatch_expression(*method.object, false);
    std::string pre_code =
            (obj_code.pre_code.empty() ? "" : obj_code.pre_code + "\n") + obj_id + " = " + obj_code.code + "\n";
    std::string code = obj_id + "." + method.method_name;
    return PythonOutputCode(pre_code, code);
}

std::string clean_path(Path a, Path b) {
    return a.as_str();
    std::string ff = a.as_str().substr(0, b.as_str().size() + 1);
    if (ff == b.as_str() + ".") {
        return a.as_vec().back();
    }
    return a.as_str();
}

PythonOutputCode PythonTranspiler::transpile_const_function(const sem::ConstFunction& function) {
    return PythonOutputCode("", clean_path(function.path, this->module_path));
}

PythonOutputCode PythonTranspiler::transpile_object_constructor(const sem::ObjectConstructor& constructor) {
    std::string code = clean_path(constructor.class_path, this->module_path);
    return PythonOutputCode("", code);
}

PythonOutputCode PythonTranspiler::transpile_call_exp(const sem::CallExp& node) {
    std::string pre_code;
    VectorOfStrings arg_names;
    std::string arg_list;
    std::string fun_id = "function_to_call_" + std::to_string(this->next_arg_n());
    PythonOutputCode func = this->dispatch_expression(*node.function, true);
    pre_code += (func.pre_code.empty() ? "" : func.pre_code + "\n") + fun_id + " = " + func.code + "\n";
    for (auto& arg: node.arguments) {
        PythonOutputCode arg_code = this->dispatch_expression(*arg, false);
        std::string arg_id = "arg_" + std::to_string(this->next_arg_n());
        pre_code += (arg_code.pre_code.empty() ? "" : arg_code.pre_code + "\n") + arg_id + " = " + arg_code.code + "\n";
        arg_list += arg_id + ", ";
    }
    std::string post_code;
    post_code += fun_id + "(";
    post_code += arg_list.empty() ? "" : arg_list.substr(0, arg_list.size() - 2);
    post_code += ")";
    return PythonOutputCode(pre_code, post_code);
}

PythonOutputCode PythonTranspiler::transpile_for(const sem::For& node) {
    PythonOutputCode exp_code = this->dispatch_expression(*node.expression, false);
    std::string exp_id = "exp_" + std::to_string(this->next_arg_n());
    std::string code = pre_if_any(exp_code) + exp_id + " = " + exp_code.code + "\n";
    code += "for " + node.varname + " in " + exp_id + ".elems:\n";
    PythonOutputCode body_code = this->transpile_block(*node.body);
    code += indent_paragraph(body_code.code, 4);
    return PythonOutputCode("", code);
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

std::string pre_if_any(const PythonOutputCode& c) {
    return c.pre_code.empty() ? "" : c.pre_code + "\n";
}

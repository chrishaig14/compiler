//
// Created by chris on 31/8/21.
//

#include "PythonModuleTranspiler.h"
#include <simple_nodes/common/include/Throw.h>
#include <simple_nodes/top/include/InstanceDef.h>
#include <simple_nodes/expressions/include/StaticMethodFromInstance.h>
#include <simple_nodes/expressions/include/Union.h>
#include <simple_nodes/expressions/include/Option.h>
#include <units/infos/Module.h>

std::string PythonModuleTranspiler::make_full_instance_name(Path instance_path, Path class_path) {
    return "T_" + this->clean_path(instance_path) + "_C_" + this->clean_path(class_path);
}

PythonExpressionOutputCode::PythonExpressionOutputCode(const std::string& pre_code, const std::string& code) : pre_code(
        pre_code), code(code) {
}

//
// Created by chris on 4/4/21.
//



PythonOutputCode PythonModuleTranspiler::transpile_declaration(const sem::Declaration& node) {
    PythonExpressionOutputCode exp_out = this->dispatch_expression(*node.expression, false);
    std::string code = exp_out.pre_code.empty() ? "" : exp_out.pre_code + "\n";
    code += node.identifier + " = " + exp_out.code;
    return code;
}

PythonOutputCode PythonModuleTranspiler::transpile_assignment(const sem::Assignment& node) {
    PythonExpressionOutputCode lvalue = this->dispatch_expression(*node.lvalue, false);
    PythonExpressionOutputCode rvalue = this->dispatch_expression(*node.rvalue, false);
    std::string code = pre_if_any(lvalue) + pre_if_any(rvalue);
    code += lvalue.code + SPACE + ASSIGN + SPACE + rvalue.code;
    return code;
}

PythonOutputCode PythonModuleTranspiler::transpile_return(const sem::Return& node) {
    if (node.expression == nullptr) {
        return "return None\n";
    }
    PythonExpressionOutputCode exp = this->dispatch_expression(*node.expression, false);
    std::string code = (exp.pre_code.empty() ? "" : exp.pre_code + "\n") + "rv = " + exp.code + "\n" + "return rv\n";
    return code;
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

PythonExpressionOutputCode PythonModuleTranspiler::transpile_id(const sem::Id& node) {
    // if (node.identifier == "") {
    //     throw std::runtime_error("Error: tranpiling empty idnode!");
    // }
    std::string id = node.identifier;
    if (id == "this" && this->add_self) {
        id = "self";
    }
    if (id == "it" && this->replace_it) {
        id = "tern.value";
    }
    return PythonExpressionOutputCode("", id);
}

PythonOutputCode PythonModuleTranspiler::transpile_function(const sem::FunctionDef& node) {
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
    for (auto& t: node.instances) {
        for (auto& i: t.second) {
            parameters += make_full_instance_name(Path(i), Path(t.first)) + COMMA + SPACE;
        }
    }
    parameters = parameters.substr(0, parameters.size() - 2);
    f_source += (node.identifier) + LPAREN + parameters + RPAREN + ":" + NEWLINE;
    // this->source += f_source;
    f_source += indent_paragraph(this->transpile_block(node.body), 4);
    return f_source;
}

PythonOutputCode PythonModuleTranspiler::transpile_block(const sem::Block& node) {
    std::string pre_code;
    std::string code;
    this->indent();
    for (auto& n: node.nodes) {
        PythonOutputCode statement_out = this->dispatch_common(*n);
        code += statement_out + "\n";
    }
    if (code.back() == '\n') {
        code = code.substr(0, code.size() - 1);
    }
    this->unindent();
    return code;
}

void PythonModuleTranspiler::transpile_program(const sem::Module& node) {
    for (auto& n: node.nodes) {
        this->dispatch_top(*n);
    }
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_integer(const sem::Integer& node) {
    return PythonExpressionOutputCode("", "Integer(" + node.str + ")");
}

PythonOutputCode PythonModuleTranspiler::transpile_call(const sem::Call& node) {
    std::string pre_code;
    VectorOfStrings arg_names;
    std::string arg_list;
    PythonExpressionOutputCode func = this->dispatch_expression(*node.function, false);
    std::string fun_id = "function_to_call_" + std::to_string(this->next_arg_n());
    pre_code += (func.pre_code.empty() ? "" : func.pre_code + "\n") + fun_id + " = " + func.code + "\n";
    for (auto& arg: node.arguments) {
        PythonExpressionOutputCode arg_code = this->dispatch_expression(*arg, false);
        std::string arg_id = "arg_" + std::to_string(this->next_arg_n());
        pre_code += (arg_code.pre_code.empty() ? "" : arg_code.pre_code + "\n") + arg_id + " = " + arg_code.code + "\n";
        arg_list += arg_id + ", ";
    }
    std::string post_code = pre_code;
    post_code += fun_id + "(";
    post_code += arg_list.empty() ? "" : arg_list.substr(0, arg_list.size() - 2);
    post_code += ")";
    return post_code;
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_string(const sem::String& node) {
    return PythonExpressionOutputCode("", "String" + LPAREN + QUOTE + node.s + QUOTE + RPAREN);
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_boolean(const sem::Bool& node) {
    return PythonExpressionOutputCode("", std::string("Boolean(") + (node.v ? "True" : "False") + ")");
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_float(const sem::Float& node) {
    return PythonExpressionOutputCode("", "MAKE_FLOAT(" + node.str + ")");
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_new(const sem::NewObject& node) {
    std::string out;
    std::string class_id = (node.class_name);
    out += "NEW(" + class_id + COMMA + SPACE;
    if (class_id == "core_D_core_D_List") {
        out += "-------{";
    }
    for (auto& m: node.args) {
        if (m != nullptr) {

            PythonExpressionOutputCode arg_code = this->dispatch_expression(*m, false);
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
    return PythonExpressionOutputCode("", out);
}

PythonOutputCode PythonModuleTranspiler::transpile_class(const sem::KlassDef& node) {
    std::string class_name = node.identifier;
    std::string code;
    code += "class " + class_name + ":\n";
    std::string def = "def __init__(self, ";
    for (const auto& m: node.attributes) {
        def += m + ", ";
    }
    def += "):\n";
    std::string block;
    for (size_t i = 0; i < node.attributes.size(); i++) {
        auto m = node.attributes[i];
        block += "self." + m + " = " + m + " #" + node.attribute_types[i] + "\n";
    }
    block = indent_paragraph(block, 4);

    def += block;
    code += indent_paragraph(def, 4) + "\n";

    for (auto& m: node.methods) {
        this->add_self = true;
        PythonOutputCode fcode = this->transpile_function(m);
        this->add_self = false;
        code += indent_paragraph(fcode, 4) + "\n";
        // std::cout << fcode.code << std::endl;
    }

    for (auto& m: node.static_methods) {
        PythonOutputCode fcode = this->transpile_function(m);
        code += indent_paragraph(fcode, 4) + "\n";
        // std::cout << fcode.code << std::endl;
    }

    return code;
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_object_member(const sem::ObjectMember& node) {
    PythonExpressionOutputCode object = this->dispatch_expression(*node.object, false);
    std::string obj_id = "obj_" + std::to_string(this->next_arg_n());
    std::string pre_code =
            (object.pre_code.empty() ? "" : object.pre_code + "\n") + obj_id + " = " + object.code + "\n";
    std::string code = obj_id + "." + node.member_name;
    return PythonExpressionOutputCode(pre_code, code);
}

PythonOutputCode PythonModuleTranspiler::transpile_while(const sem::While& node) {
    PythonExpressionOutputCode cond = this->dispatch_expression(*node.condition, false);
    std::string cond_id = "condition_" + std::to_string(this->next_arg_n());
    PythonOutputCode thenc = this->transpile_block(node.body);
    std::string out = pre_if_any(cond) + cond_id + " = " + cond.code + "\n";
    out += "while " + cond_id + ":\n";
    out += indent_paragraph(thenc, 4) + "\n";
    out += indent_paragraph(pre_if_any(cond) + cond_id + " = " + cond.code, 4);
    return out;

}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_list(const sem::List& node) {
    std::string pre_code;
    std::string elem_ids;
    for (auto& e: node.elements) {
        PythonExpressionOutputCode element_code = this->dispatch_expression(*e, false);
        std::string elem_id = "elem_" + std::to_string(this->next_arg_n());
        pre_code += pre_if_any(element_code) + elem_id + " = " + element_code.code + "\n";
        elem_ids += elem_id + ", ";
    }
    if (not elem_ids.empty()) {
        elem_ids = elem_ids.substr(0, elem_ids.size() - 2);
    }
    std::string code = "List([" + elem_ids + "])";
    return PythonExpressionOutputCode(pre_code, code);
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_dict(const sem::Dict& node) {
    std::string pre;
    std::string out = "Dict({";
    for (auto& i: node.items) {
        auto key = this->dispatch_expression(*i.first, false);
        auto value = this->dispatch_expression(*i.second, false);
        pre += key.pre_code + "\n";
        std::string key_id = "key_" + std::to_string(this->next_arg_n());
        std::string value_id = "value_" + std::to_string(this->next_arg_n());
        pre += key_id + " = " + key.code + "\n";
        pre += value.pre_code + "\n";
        pre += value_id + " = " + value.code + "\n";
        out += key_id + ":" + value_id + ", ";
    }
    out += "})";
    return PythonExpressionOutputCode(pre, out);
}


PythonOutputCode PythonModuleTranspiler::transpile_if(const sem::If& node) {
    std::string code;
    PythonExpressionOutputCode cond = this->dispatch_expression(node.condition, false);
    PythonOutputCode thenc = this->transpile_block(node.then);
    std::string cond_id = "condition_" + std::to_string(this->next_arg_n());
    code += cond.pre_code.empty() ? "" : cond.pre_code + "\n";
    code += cond_id + " = " + cond.code + "\n";
    code += "if" + SPACE + cond_id + ":" + NEWLINE + indent_paragraph(thenc, 4);
    if (node._else != nullptr) {
        code += NEWLINE + "else:" + NEWLINE + indent_paragraph(this->transpile_block(*node._else), 4);
    }
    return code;
}

PythonOutputCode PythonModuleTranspiler::transpile_break(const sem::Break& node) {
    return "break";
}

PythonOutputCode PythonModuleTranspiler::transpile_continue(const sem::Continue& node) {
    return "continue";
}

PythonOutputCode PythonModuleTranspiler::transpile_match(const sem::Match& node) {
    std::string out;
    PythonExpressionOutputCode exp_out = this->dispatch_expression(*node.exp, false);
    out += exp_out.pre_code;
    out += "union_object = " + exp_out.code + NEWLINE;
    out += "actual_union_type = union_object[0]" + NEWLINE;
    out += "actual_union_object = union_object[1]" + NEWLINE;
    out += "if actual_union_type == " + std::to_string(node.cases[0].index) + ":\n";
    std::string first_case_body = this->transpile_block(node.cases[0].body);
    out += indent_paragraph(node.cases[0].var_name + " = actual_union_object", 4) + NEWLINE;
    out += indent_paragraph(first_case_body, 4);
    bool first = true;
    for (auto& kase: node.cases) {
        if (first) {
            first = false;
            continue;
        }
        out += NEWLINE + "elif actual_union_type == " + std::to_string(kase.index) + ":\n";
        std::string case_body = this->transpile_block(kase.body);
        out += indent_paragraph(kase.var_name + " = actual_union_object", 4) + NEWLINE;
        out += indent_paragraph(case_body, 4);
    }
    return out;
}

PythonOutputCode PythonModuleTranspiler::transpile_enum(const sem::EnumDef& node) {
    std::string values;
    size_t i = 0;
    for (auto v: node.values) {
        values += v + " = " + std::to_string(i++) + "\n";
    }
    std::string code = "class " + node.id + ":\n";
    std::string eq = "@staticmethod\ndef __eq__(a, b):\n    return a == b";
    code += indent_paragraph(values + "\n" + eq, 4);
    return code;
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_enum_member(const sem::EnumMember& node) {
    std::string out = this->clean_path(node.enum_path) + "." + node.value;
    return PythonExpressionOutputCode("", out);
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_ternary(const sem::Ternary& node) {
    std::string out;
    PythonExpressionOutputCode tern = this->dispatch_expression(*node.exp, false);
    out = tern.pre_code;
    std::string pre = "tern = " + tern.code + NEWLINE;
    this->replace_it = true;
    PythonExpressionOutputCode truec = this->dispatch_expression(*node.true_case, false);
    out += truec.pre_code;
    PythonExpressionOutputCode falsec = this->dispatch_expression(*node.false_case, false);
    this->replace_it = false;
    out += falsec.pre_code;
    out += LPAREN + truec.code + " if tern.value is not None else " + falsec.code + RPAREN;
    return PythonExpressionOutputCode(pre, out);
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_none(const sem::None& node) {
    return PythonExpressionOutputCode("", "None");
}

PythonOutputCode PythonModuleTranspiler::transpile_try_catch(const sem::TryCatch& node) {
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
    return "";
}

std::string PythonModuleTranspiler::transpile_module(const sem::Module& block, Path path) {
    this->module_path = path;
    std::string code;
    for (auto& n: block.nodes) {
        PythonOutputCode definition_output = this->dispatch_top(*n);
        code += definition_output + "\n";
    }
    return code;
}

PythonOutputCode PythonModuleTranspiler::dispatch_top(const sem::Top& node) {
    switch (node.type) {
        case sem::TopType::FUNCTION:
            return this->transpile_function((const sem::FunctionDef&) (node));
        case sem::TopType::ENUM:
            return this->transpile_enum((const sem::EnumDef&) node);
        case sem::TopType::CLASS:
            return this->transpile_class((const sem::KlassDef&) node);
        case sem::TopType::TYPECLASS:
            return "";
            break;
        case sem::TopType::INSTANCE:
            return this->transpile_instance((const sem::InstanceDef&) node);
    }
    __builtin_unreachable();
}

PythonOutputCode PythonModuleTranspiler::dispatch_common(const sem::Common& node) {
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

PythonExpressionOutputCode PythonModuleTranspiler::dispatch_expression(const sem::Exp& node, bool called_function) {
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
            return this->transpile_object_method(static_cast<const sem::ObjectMethod&>(node), called_function);
        case sem::ExpType::CONST_FUNCTION:
            return this->transpile_const_function(static_cast<const sem::ConstFunction&>(node));
        case sem::ExpType::OBJECT_CONSTRUCTOR:
            return this->transpile_object_constructor(static_cast<const sem::ObjectConstructor&>(node));
        case sem::ExpType::STATIC_METHOD:
            return this->transpile_static_method(static_cast<const sem::StaticMethod&>(node), called_function);
            break;
        case sem::ExpType::INSTANCE_OBJECT:
            break;
        case sem::ExpType::METHOD_FROM_INSTANCE: {
            auto& n = static_cast<const sem::ObjectMethodFromInstance&>(node);
            PythonExpressionOutputCode object_out = this->dispatch_expression(*n.object, false);
            std::string out_pre_code = object_out.pre_code + "\nobject = " + object_out.code;
            std::string out_code = make_full_instance_name(n.instance.typeclass_path, n.instance.class_path);
            return PythonExpressionOutputCode(out_pre_code, out_code);
        }
        case sem::ExpType::STATIC_METHOD_FROM_INSTANCE: {
            auto& n = static_cast<const sem::StaticMethodFromInstance&>(node);
            std::string out_code = make_full_instance_name(n.instance.typeclass_path, n.instance.class_path);
            return PythonExpressionOutputCode("", out_code);
        }
        case sem::ExpType::UNION:
            return this->transpile_union(static_cast<const sem::Union&>(node));
        case sem::ExpType::OPTION:
            return this->transpile_option(static_cast<const sem::Option&>(node));
    }
    __builtin_unreachable();
}

size_t PythonModuleTranspiler::next_arg_n() {
    return this->arg_n++;
}

void PythonModuleTranspiler::indent() {
    this->indent_level += 4;
}

std::string PythonModuleTranspiler::indentation() {
    return std::string(this->indent_level, ' ');
}

void PythonModuleTranspiler::unindent() {
    this->indent_level -= 4;
}

PythonModuleTranspiler::PythonModuleTranspiler(Module& module) : module(module) {
    this->indent_level = 0;
    this->arg_n = 0;
    this->add_self = false;
    this->replace_it = false;
}

PythonExpressionOutputCode
PythonModuleTranspiler::transpile_object_method(const sem::ObjectMethod& method, bool called_function) {
    std::string obj_id = "obj_" + std::to_string(this->next_arg_n());
    PythonExpressionOutputCode obj_code = this->dispatch_expression(*method.object, false);
    std::string pre_code =
            (obj_code.pre_code.empty() ? "" : obj_code.pre_code + "\n") + obj_id + " = " + obj_code.code + "\n";
    std::string code = obj_id + "." + method.method_name;
    return PythonExpressionOutputCode(pre_code, code);
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_static_method(const sem::StaticMethod& method, bool b) {
    return PythonExpressionOutputCode("", this->clean_path(method.class_path) + "." + method.method_name);
}

std::string PythonModuleTranspiler::clean_path(Path path) {
    auto it = this->module.imported_paths_no_alias.find(path.basname());
    if (it != this->module.imported_paths_no_alias.end()) {
        return path.basname();
    }
    std::string code;
    auto v = path.as_vec();
    v.pop_back();
    it = this->module.imported_paths_no_alias.find(v.back());
    if (it != this->module.imported_paths_no_alias.end()) {
        code = v.back() + "." + path.basname();
        return code;
    }
    auto p = path.as_vec();
    p.pop_back();
    if (p == this->module.path.as_vec()) {
        code = path.basname();
    }
    return code;
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_const_function(const sem::ConstFunction& function) {
    return PythonExpressionOutputCode("", this->clean_path(function.path));
}

PythonExpressionOutputCode
PythonModuleTranspiler::transpile_object_constructor(const sem::ObjectConstructor& constructor) {
    return PythonExpressionOutputCode("", this->clean_path(constructor.class_path));
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_call_exp(const sem::CallExp& node) {
    std::string pre_code;
    VectorOfStrings arg_names;
    std::string arg_list;
    std::string fun_id = "function_to_call_" + std::to_string(this->next_arg_n());
    PythonExpressionOutputCode func("", "");
    switch (node.function->type) {
        case sem::ExpType::OBJECT_METHOD: {
            auto& f = static_cast<sem::ObjectMethod&>(*node.function);
            PythonExpressionOutputCode arg_code = this->dispatch_expression(*f.object, false);
            std::string arg_id = "arg_" + std::to_string(this->next_arg_n());
            pre_code +=
                    (arg_code.pre_code.empty() ? "" : arg_code.pre_code + "\n") + arg_id + " = " + arg_code.code + "\n";
            arg_list += arg_id + ", ";
            func.code = this->clean_path(f.class_path) + "." + f.method_name;
            break;
        }
        case sem::ExpType::METHOD_FROM_INSTANCE: {
            auto& f = static_cast<sem::ObjectMethodFromInstance&>(*node.function);
            PythonExpressionOutputCode arg_code = this->dispatch_expression(*f.object, false);
            std::string arg_id = "arg_" + std::to_string(this->next_arg_n());
            pre_code +=
                    (arg_code.pre_code.empty() ? "" : arg_code.pre_code + "\n") + arg_id + " = " + arg_code.code + "\n";
            arg_list += arg_id + ", ";
            func.code = make_full_instance_name(f.instance.typeclass_path, f.instance.class_path) + "[\"" + f.method +
                        "\"]";
            break;
        }
        case sem::ExpType::STATIC_METHOD_FROM_INSTANCE: {
            auto& f = static_cast<sem::StaticMethodFromInstance&>(*node.function);
            func.code = make_full_instance_name(f.instance.typeclass_path, f.instance.class_path) + "[\"" + f.method +
                        "\"]";
            break;
        }
        default: {
            func = this->dispatch_expression(*node.function, true);
        }
    }
    pre_code += (func.pre_code.empty() ? "" : func.pre_code + "\n") + fun_id + " = " + func.code + "\n";
    for (auto& arg: node.arguments) {
        PythonExpressionOutputCode arg_code = this->dispatch_expression(*arg, false);
        std::string arg_id = "arg_" + std::to_string(this->next_arg_n());
        pre_code += (arg_code.pre_code.empty() ? "" : arg_code.pre_code + "\n") + arg_id + " = " + arg_code.code + "\n";
        arg_list += arg_id + ", ";
    }
    for (auto& arg: node.instances) {
        arg_list += make_full_instance_name(arg.typeclass_path, arg.class_path) + ", ";
    }
    std::string post_code;
    post_code += fun_id + "(";
    post_code += arg_list.empty() ? "" : arg_list.substr(0, arg_list.size() - 2);
    post_code += ")";
    return PythonExpressionOutputCode(pre_code, post_code);
}

PythonOutputCode PythonModuleTranspiler::transpile_for(const sem::For& node) {
    PythonExpressionOutputCode exp_code = this->dispatch_expression(*node.expression, false);
    std::string exp_id = "exp_" + std::to_string(this->next_arg_n());
    std::string code = pre_if_any(exp_code) + exp_id + " = " + exp_code.code + "\n";
    code += "for " + node.varname + " in " + exp_id + ".elems:\n";
    PythonOutputCode body_code = this->transpile_block(node.body);
    code += indent_paragraph(body_code, 4);
    return code;
}

PythonOutputCode PythonModuleTranspiler::transpile_instance(const sem::InstanceDef& def) {
    std::string code;
    std::string full_instance_name = make_full_instance_name(def.instance_path, def.base_type_path);
    std::string post;
    for (auto& sem_method: def.methods) {
        auto& m = sem_method.func;
        std::string full_method_name = full_instance_name + "_M_" + m.identifier;
        post += "\"" + m.identifier + "\": " + full_method_name + ",\n";
        auto renamed = m;
        renamed.identifier = full_method_name;
        if (not sem_method.is_static) {
            this->add_self = true;
        }
        code += this->transpile_function(renamed);
        this->add_self = false;
        code += "\n";
    }
    this->add_self = false;

    code += full_instance_name;
    code += " = {\n";
    code += post;
    code += "}";
    return code;
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_union(const sem::Union& an_union) {
    PythonExpressionOutputCode exp = this->dispatch_expression(*an_union.exp, false);
    std::string pre_code = exp.pre_code + NEWLINE + "union_value = " + exp.code + NEWLINE;
    std::string code = "(" + std::to_string(an_union.type_index) + ", union_value)";
    return PythonExpressionOutputCode(pre_code, code);
}

PythonExpressionOutputCode PythonModuleTranspiler::transpile_option(const sem::Option& option) {
    PythonExpressionOutputCode out("", "");

    if (option.exp != nullptr) {
        auto exp = this->dispatch_expression(*option.exp, false);
        out.pre_code = exp.pre_code;
        out.code = "Option(" + exp.code + ")";
    } else {
        out.code = "Option(None)";
    }
    return out;
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

std::string pre_if_any(const PythonExpressionOutputCode& c) {
    return c.pre_code.empty() ? "" : c.pre_code + "\n";
}

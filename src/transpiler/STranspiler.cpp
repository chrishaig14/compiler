//
// Created by chris on 4/4/21.
//

#include "STranspiler.h"
#include "../simple_nodes/ThrowSNode.h"

OutputCode STranspiler::transpile_declaration(DeclarationSNode& node) {
    OutputCode exp = this->dispatch(*node.expression);
    std::string out;
    out += exp.pre_code;
    out += TOBJECT + SPACE + node.identifier + SPACE + ASSIGN + SPACE + GCDECLARE + LPAREN + exp.code + RPAREN + SEMIC +
           NEWLINE;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_assignment(AssignmentSNode& node) {
    std::string out;
    OutputCode lvalue = this->dispatch(*node.lvalue);
    out += lvalue.pre_code;
    OutputCode rvalue = this->dispatch(*node.rvalue);
    out += rvalue.pre_code;
    out += lvalue.code + SPACE + ASSIGN + SPACE + GCASSIGN + LPAREN + lvalue.code + COMMA + SPACE + rvalue.code +
           RPAREN + SEMIC + NEWLINE;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_return(ReturnSNode& node) {
    if (node.expression == nullptr) {
        return OutputCode("", "return nullptr;");
    }
    OutputCode exp = this->dispatch(*node.expression);
    std::string out = exp.pre_code;
    out += RETURN_VAR + SPACE + ASSIGN + SPACE + GCRETURN + LPAREN + SPACE + exp.code + RPAREN + SEMIC + NEWLINE;
    for (auto reachable: node.reachables) {
        if (reachable == "this") {
            reachable = "this_obj";
        }
        out += GCOUTOFSCOPE + LPAREN + reachable + RPAREN + SEMIC + NEWLINE;
    }
    out += RETURN + SPACE + RETURN_VAR + SEMIC + NEWLINE;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_throw(ThrowSNode& node) {
    OutputCode exp = this->dispatch(*node.expression);
    std::string out = exp.pre_code;
    out += RETURN_VAR + SPACE + ASSIGN + SPACE + "set_tag(" + LPAREN + SPACE + exp.code + RPAREN + ",EXCEPTION_TAG)" +
           SEMIC + NEWLINE;
    out += RETURN + SPACE + RETURN_VAR + SEMIC + NEWLINE;
    return OutputCode("", out);
}

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

OutputCode STranspiler::transpile_id(IdSNode& node) {
    if (node.identifier == "") {
        throw std::runtime_error("Error: tranpiling empty idnode!");
    }
    std::string out;
    if (node.identifier == "this") {
        node.identifier = "this_obj";
    }
    out += path_to_id(node.identifier);
    return OutputCode("", out);
}

void STranspiler::transpile_function(FunctionSNode& node) {
    std::string parameters;

    for (auto& param : node.params) {
        if (param == "this") {
            param = "this_obj";
        }
    }

    for (const auto& pn: node.params) {
        std::string parameter = TOBJECT + SPACE + pn;
        parameters += parameter + COMMA + SPACE;
    }
    parameters = parameters.substr(0, parameters.size() - 2);
    node.identifier = path_to_id(node.identifier);

    std::string raw_function_identifier = node.identifier + "_f";

    std::string signature = TOBJECT + SPACE + raw_function_identifier + LPAREN + parameters + RPAREN;
    this->header += signature + SEMIC + NEWLINE;
    std::string f_source = signature + LCURLY + NEWLINE;
    for (const auto& pn: node.params) {
        std::string parameter = GCDECLARE + LPAREN + pn + RPAREN + SEMIC + NEWLINE;
        f_source += parameter;
    }
    f_source += TOBJECT + SPACE + "it" + SEMIC + NEWLINE;
    f_source += TOBJECT + SPACE + RETURN_VAR + SEMIC + NEWLINE;
    OutputCode fbod = this->transpile_block(*node.body);
    f_source += fbod.code;
    f_source += RETURN + SPACE + "nullptr" + SEMIC + NEWLINE;
    f_source += RCURLY + NEWLINE;

    std::string function_class = "Function" + std::to_string(node.params.size());

    std::string function_obj_name = node.identifier;

    this->header += EXTERN + SPACE + TOBJECT + SPACE + function_obj_name + SEMIC + NEWLINE;
    this->source +=
            function_class + SPACE + raw_function_identifier + "_o" + SPACE + ASSIGN + SPACE + function_class + SPACE +
            LPAREN + raw_function_identifier + RPAREN + SEMIC + NEWLINE;
    this->source += TOBJECT + SPACE + function_obj_name + "=FTAG(&" + raw_function_identifier + "_o);\n";
    this->source += f_source;
}

OutputCode STranspiler::transpile_block(BlockSNode& node) {
    std::string out;
    for (auto* n: node.nodes) {
        OutputCode nod = this->dispatch(*n);
        out += nod.pre_code;
        if (n->type == SNodeType::CALL) {
            std::string temp_name = "temp_" + std::to_string(rand());
            out += TOBJECT + SPACE + temp_name + ASSIGN + nod.code;
            out += SEMIC + NEWLINE;
            // if (this->in_try_catch) {
            //     out += "if" + SPACE + LPAREN + "has_tag" + LPAREN + temp_name + COMMA + SPACE + "EXCEPTION_TAG" +
            //            RPAREN + RPAREN + SPACE + LCURLY + "thrown_exception" + SPACE + ASSIGN + SPACE + temp_name +
            //            SEMIC + RCURLY + NEWLINE;
            // } else {
            //     out += "if" + SPACE + LPAREN + "has_tag" + LPAREN + temp_name + COMMA + SPACE + "EXCEPTION_TAG" +
            //            RPAREN + RPAREN + SPACE + LCURLY + RETURN + SPACE + temp_name + SEMIC + RCURLY + NEWLINE;
            // }
        } else {
            out += nod.code;
        }
    }
    for (auto local: node.locals) {
        if (local == "this") {
            local = "this_obj";
        }
        out += GCOUTOFSCOPE + LPAREN + local + RPAREN + SEMIC + NEWLINE;
    }
    return OutputCode("", out);
}

void STranspiler::transpile_program(BlockSNode& node) {
    for (auto* n: node.nodes) {
        this->dispatch_top(*n);
    }
}

OutputCode STranspiler::transpile_integer(IntegerSNode& node) {
    return OutputCode("", "MAKE_INT" + LPAREN + node.str + RPAREN);
}

OutputCode STranspiler::transpile_call(CallSNode& node) {
    std::string pre_code;
    VectorOfStrings arg_names;
    std::string fofo;
    for (auto* arg: node.arguments) {
        std::string afofo;
        OutputCode arg_code = this->dispatch(*arg);
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
    OutputCode func = this->dispatch(*node.function);
    pre_code += func.pre_code;
    post_code += "CALL" + std::to_string(node.arguments.size()) + "(" + func.code;
    post_code += COMMA + SPACE;
    post_code += fofo;
    post_code = post_code.substr(0, post_code.size() - 2);
    post_code += ")";
    // std::cout << "POST CODE:" << std::endl;
    // std::cout << post_code << std::endl;

    return OutputCode(pre_code, post_code);

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

OutputCode STranspiler::transpile_string(StringSNode& node) {
    return OutputCode("", "MAKE_STRING" + LPAREN + QUOTE + node.s + QUOTE + RPAREN);
}

OutputCode STranspiler::transpile_boolean(BoolSNode& node) {
    return OutputCode("", node.v ? "TRUE" : "FALSE");
}

OutputCode STranspiler::transpile_float(FloatSNode& pNode) {
    return OutputCode("", "MAKE_FLOAT(" + pNode.str + ")");
}

void STranspiler::transpile_class(ClassSNode& node) {
    std::string out;
    std::string class_name = path_to_id(node.identifier);
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

OutputCode STranspiler::transpile_new(NewObjectSNode& node) {
    std::string out;
    std::string class_id = path_to_id(node.class_name);
    out += "NEW(" + class_id + COMMA + SPACE;
    if (class_id == "core_D_core_D_List") {
        out += "-------{";
    }
    for (auto* m: node.args) {
        if (m != nullptr) {

            OutputCode arg_code = this->dispatch(*m);
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
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_object_member(ObjectMemberSNode& node) {
    std::string out;
    OutputCode object = this->dispatch(*node.object);
    out += object.pre_code;
    out += "CAST" + LPAREN + object.code + COMMA + SPACE + path_to_id(node.class_path.as_str()) + RPAREN + "->" +
           node.member_name;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_while(WhileSNode& node) {
    std::string out;
    OutputCode cond_out = this->dispatch(*node.condition);
    OutputCode body_out = this->transpile_block(*node.body);
    std::string condition_name = "cond_" + std::to_string(rand());
    out += cond_out.pre_code;
    out += TOBJECT + SPACE + condition_name + SPACE + ASSIGN + SPACE + cond_out.code + SEMIC + NEWLINE;
    out += "while" + SPACE + LPAREN + "GET_BOOL" + LPAREN + condition_name + RPAREN + RPAREN + SPACE + LCURLY +
           body_out.code + condition_name + SPACE + ASSIGN + SPACE + cond_out.code + SEMIC + NEWLINE + RCURLY;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_list(ListSNode& node) {
    std::string out;
    out = "NEW(XList,{";
    for (auto* e: node.elements) {
        OutputCode el = this->dispatch(*e);
        out += el.pre_code;
        out += el.code + ", ";
    }
    if (!node.elements.empty()) {
        out = out.substr(0, out.size() - 2);
    }
    out += "})";
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_dict(DictSNode& node) {
    std::string out;
    out = "NEW(XDict,{";
    if (node.items.size() == 1) {
        auto e = node.items[0];
        OutputCode key = this->dispatch(*e.first);
        OutputCode value = this->dispatch(*e.second);
        out += key.pre_code;
        out += value.pre_code;
        out += "std::make_pair(" + key.code + ", " + value.code + ")" + SPACE;
    }
    if (node.items.size() > 1) {
        for (auto e: node.items) {
            OutputCode key = this->dispatch(*e.first);
            OutputCode value = this->dispatch(*e.second);
            out += key.pre_code;
            out += value.pre_code;
            out += "{" + key.code + ", " + value.code + "}" + COMMA + SPACE;
        }
        out = out.substr(0, out.size() - 2);
    }
    out += "})";
    return OutputCode("", out);
}


OutputCode STranspiler::transpile_if(IfSNode& node) {
    std::string pre;
    std::string out;
    OutputCode cond = this->dispatch(*node.condition);
    out += cond.pre_code;
    std::string condition_name = "cond_" + std::to_string(rand());
    if (node.condition->type == SNodeType::CALL) {
        out += TOBJECT + SPACE + condition_name + SPACE + ASSIGN + cond.code + SEMIC + NEWLINE;
    }
    OutputCode thenc = this->transpile_block(*node.then);
    out += "if" + SPACE + LPAREN + "GET_BOOL" + LPAREN + condition_name + RPAREN + RPAREN + LCURLY + NEWLINE +
           thenc.code + RCURLY;
    for (auto elif: node.elifs) {
        OutputCode elifc = this->dispatch(*elif.first);
        out += elifc.pre_code;
        OutputCode elifb = this->transpile_block(*elif.second);
        out += "else if" + SPACE + LPAREN + "GET_BOOL" + LPAREN + elifc.code + RPAREN + RPAREN + LCURLY + NEWLINE +
               elifb.code + RCURLY;
    }
    if (node._else != nullptr) {
        OutputCode _else = this->transpile_block(*node._else);
        out += _else.pre_code;
        out += "else" + SPACE + LCURLY + NEWLINE + _else.code + NEWLINE + RCURLY;
    }
    out += NEWLINE;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_break(BreakSNode& node) {
    std::string out;
    for (auto reachable: node.reachables) {
        if (reachable == "this") {
            reachable = "this_obj";
        }
        out += GCOUTOFSCOPE + LPAREN + reachable + RPAREN + SEMIC + NEWLINE;
    }
    out += "break" + SEMIC + NEWLINE;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_continue(ContinueSNode& node) {
    std::string out;
    for (auto reachable: node.reachables) {
        if (reachable == "this") {
            reachable = "this_obj";
        }
        out += GCOUTOFSCOPE + LPAREN + reachable + RPAREN + SEMIC + NEWLINE;
    }
    out += "continue" + SEMIC + NEWLINE;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_match(MatchSNode& node) {
    std::string out;
    OutputCode exp = this->dispatch(*node.exp);
    out += exp.pre_code;
    out += exp.code;
    out += "switch" + SPACE + LPAREN + "GET_INT(CAST(" + node.varname + ",core_D_core_D_Union" + RPAREN + "->type" +
           RPAREN + RPAREN + SPACE + LCURLY;
    for (auto c: node.cases) {
        OutputCode mc = this->transpile_block(*c.second);
        out += "case" + SPACE + "" + std::to_string(c.first) + "" + SPACE + ":" + SPACE + LCURLY + mc.code + "break" +
               SEMIC + RCURLY;
    }
    out += RCURLY;
    return OutputCode("", out);
}

void STranspiler::transpile_enum(EnumSNode& node) {
    std::string out;
    std::string enum_name = path_to_id(node.id);
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

OutputCode STranspiler::transpile_enum_member(EnumMemberSNode& node) {
    std::string out;
    out += path_to_id(node.enum_name) + "_" + node.value;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_ternary(TernarySNode& node) {
    std::string out;
    OutputCode tern = this->dispatch(*node.ext);
    out = tern.pre_code;
    OutputCode truec = this->dispatch(*node.true_case);
    out += truec.pre_code;
    OutputCode falsec = this->dispatch(*node.false_case);
    out += falsec.pre_code;
    out += LPAREN + "(it=" + tern.code + ")!=nullptr? " + truec.code + SPACE + ":" + SPACE + falsec.code + RPAREN;
    return OutputCode("", out);
}

OutputCode STranspiler::transpile_none(NoneSNode& nn) {
    return OutputCode("", "nullptr");
}

OutputCode STranspiler::transpile_try_catch(TryCatchSNode& node) {
    this->in_try_catch = true;
    OutputCode body_out = this->transpile_block(*node.body);
    this->in_try_catch = false;
    std::string out = "TaggedObject* thrown_exception = nullptr;\n" + body_out.code;
    out += "if (thrown_exception!=nullptr){\n";
    for (size_t i = 0; i < node.catches_bodies.size(); i++) {
        OutputCode catch_out = this->transpile_block((BlockSNode&) *node.catches_bodies[i]);
        out += "if (UNTAG(thrown_exception)->class_name==\"" + path_to_id(node.e_names_types[i].second) +
               "\"){TaggedObject*" + node.e_names_types[i].first + "=thrown_exception;\n" + catch_out.code + "} else ";
    }
    out += RETURN + SPACE + "CALL1(test_D_bootstrap_D_Exception_D___init__,test_D_bootstrap_D_Exception_D___init__)" +
           SEMIC + NEWLINE;
    // out = out.substr(0, out.size() - 5);
    out += "\n}";
    return OutputCode("", out);
}

OutputCode::OutputCode(const std::string& pre_code, const std::string& code) : pre_code(pre_code), code(code) {
}

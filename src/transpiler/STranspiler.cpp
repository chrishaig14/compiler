//
// Created by chris on 4/4/21.
//

#include "STranspiler.h"

std::string STranspiler::transpile_declaration(DeclarationSNode* node) {
    std::string out;
    out = TOBJECT + SPACE + node->identifier + SPACE + ASSIGN + SPACE + GCDECLARE + LPAREN +
          this->dispatch(node->expression) + RPAREN + SEMIC + NEWLINE;
    return out;
}

std::string STranspiler::transpile_assignment(AssignmentSNode* node) {
    std::string out;
    std::string lvalue = this->dispatch(node->lvalue);
    out += lvalue + SPACE + ASSIGN + SPACE + GCASSIGN + LPAREN + lvalue + COMMA + SPACE + this->dispatch(node->rvalue) +
           RPAREN + SEMIC + NEWLINE;
    return out;
}

std::string STranspiler::transpile_return(ReturnSNode* node) {
    std::string out = TOBJECT + SPACE + RETURN_VAR + SPACE + ASSIGN + SPACE + GCRETURN + LPAREN + SPACE +
                      this->dispatch(node->expression) + RPAREN + SEMIC + NEWLINE;
    out += RETURN + SPACE + RETURN_VAR + SEMIC + NEWLINE;
    return out;
}

std::string path_to_id(std::string p) {
    std::string out;
    for (size_t i = 0; i < p.size(); i++) {
        char c = p[i];
        if (c == '.') {
            out += "_D_";
        } else {
            out += std::string(1, c);
        }
    }
    return out;
}

std::string STranspiler::transpile_id(IdSNode* node) {
    if (node->identifier == "") {
        throw std::runtime_error("Error: tranpiling empty idnode!");
    }
    std::string out;
    if (node->identifier == "this") {
        node->identifier = "this_obj";
    }
    out += path_to_id(node->identifier);
    return out;
}

void STranspiler::transpile_function(FunctionSNode* node) {
    std::string parameters;

    for (size_t i = 0; i < node->params.size(); i++) {
        if (node->params[i] == "this") {
            node->params[i] = "this_obj";
        }
    }

    for (auto pn: node->params) {
        std::string parameter = TOBJECT + SPACE + pn;
        parameters += parameter + COMMA + SPACE;
    }
    parameters = parameters.substr(0, parameters.size() - 2);
    node->identifier = path_to_id(node->identifier);

    std::string raw_function_identifier = node->identifier + "_f";

    std::string signature = TOBJECT + SPACE + raw_function_identifier + LPAREN + parameters + RPAREN;
    this->header += signature + SEMIC + NEWLINE;
    std::string f_source = signature + LCURLY + NEWLINE;
    for (auto pn: node->params) {
        std::string parameter = GCDECLARE + LPAREN + pn + RPAREN + SEMIC + NEWLINE;
        f_source += parameter;
    }
    f_source += TOBJECT + SPACE + "it" + SEMIC + NEWLINE;
    f_source += this->transpile_block(node->body);
    f_source += RETURN + SPACE + "nullptr" + SEMIC + NEWLINE;
    f_source += RCURLY + NEWLINE;

    std::string function_class = "Function" + std::to_string(node->params.size());

    std::string function_obj_name = node->identifier;

    this->header += EXTERN + SPACE + TOBJECT + SPACE + function_obj_name + SEMIC + NEWLINE;
    this->source +=
            function_class + SPACE + raw_function_identifier + "_o" + SPACE + ASSIGN + SPACE + function_class + SPACE +
            LPAREN + raw_function_identifier + RPAREN + SEMIC + NEWLINE;
    this->source += TOBJECT + SPACE + function_obj_name + "=FTAG(&" + raw_function_identifier + "_o);\n";
    this->source += f_source;
}

std::string STranspiler::transpile_block(BlockSNode* node) {
    std::string out;
    for (auto n: node->nodes) {
        out += this->dispatch(n);
        if (n->type == SNodeType::CALL) {
            out += SEMIC + NEWLINE;
        }
    }
    return out;
}

void STranspiler::transpile_program(BlockSNode* node) {
    for (auto n: node->nodes) {
        this->dispatch_top(n);
    }
}

std::string STranspiler::transpile_integer(IntegerSNode* node) {
    return "MAKE_INT" + LPAREN + node->str + RPAREN;
}

std::string STranspiler::transpile_call(CallSNode* node) {
    std::string out;
    std::string arguments;
    for (auto arg: node->arguments) {
        std::string arg_s = this->dispatch(arg);
        arguments += arg_s + COMMA + SPACE;
    }
    arguments = arguments.substr(0, arguments.size() - 2);
    out += "CALL" + std::to_string(node->arguments.size()) + "(" + this->dispatch(node->function);
    if (arguments.size() != 0) {
        out += +", " + arguments;
    }
    out += ")";
    return out;
}

std::string STranspiler::transpile_string(StringSNode* node) {
    return "MAKE_STRING" + LPAREN + QUOTE + node->s + QUOTE + RPAREN;
}

std::string STranspiler::transpile_boolean(BoolSNode* node) {
    return node->v ? "TRUE" : "FALSE";
}

std::string STranspiler::transpile_float(FloatSNode* pNode) {
    return "MAKE_FLOAT(" + pNode->str + ")";
}

void STranspiler::transpile_class(ClassSNode* node) {
    std::string out;
    std::string class_name = path_to_id(node->identifier);
    out += CLASS + SPACE + class_name + SPACE + ": public XObject {\n";
    out += "public: \n";
    for (auto m: node->members) {
        out += TOBJECT + SPACE + m + SEMIC + NEWLINE;
    }
    out += class_name + LPAREN;
    for (auto m: node->members) {
        out += TOBJECT + SPACE + m + COMMA + SPACE;
    }
    out = out.substr(0, out.size() - 2);
    out += RPAREN + SPACE + ":" + SPACE + "XObject" + LPAREN + QUOTE + class_name + QUOTE + RPAREN + SPACE + LCURLY +
           NEWLINE;
    for (auto m: node->members) {
        out += "this->" + m + " = " + m + SEMIC + NEWLINE;
    }
    out += RCURLY + NEWLINE;
    out += RCURLY + SEMIC + NEWLINE;
    this->header += out;
}

std::string STranspiler::transpile_new(NewObjectSNode* node) {
    std::string out;
    std::string class_id = path_to_id(node->class_name);
    out += "NEW(" + class_id + COMMA + SPACE;
    if (class_id == "core_D_List") {
        out += "{";
    }
    for (auto m: node->args) {
        if (m != nullptr) {
            out += this->dispatch(m) + COMMA + SPACE;
        } else {
            out += "nullptr" + COMMA + SPACE;
        }
    }
    if (node->args.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    if (class_id == "core_D_List") {
        out += "}";
    }
    out += RPAREN;
    return out;
}

std::string STranspiler::transpile_object_member(ObjectMemberSNode* sn) {
    std::string out;
    out += "CAST" + LPAREN + this->dispatch(sn->object) + COMMA + SPACE + path_to_id(sn->class_path.as_str()) + RPAREN +
           "->" + sn->member_name;
    return out;
}

std::string STranspiler::transpile_while(WhileSNode* sn) {
    std::string out;
    std::string cond_out = this->dispatch(sn->condition);
    std::string body_out = this->transpile_block(sn->body);
    out += "while" + SPACE + LPAREN + "GET_BOOL" + LPAREN + cond_out + RPAREN + RPAREN + SPACE + LCURLY + body_out +
           RCURLY;
    return out;
}

std::string STranspiler::transpile_list(ListSNode* ln) {
    std::string out;
    out = "NEW(XList,{";
    for (auto e: ln->elements) {
        out += this->dispatch(e) + ", ";
    }
    if (ln->elements.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += "})";
    return out;
}

std::string STranspiler::transpile_if(IfSNode* in) {
    std::string out;
    out += "if" + SPACE + LPAREN + "GET_BOOL" + LPAREN + this->dispatch(in->condition) + RPAREN + RPAREN + LCURLY +
           NEWLINE + this->transpile_block(in->then) + RCURLY;
    for (auto elif: in->elifs) {
        out += "else if" + SPACE + LPAREN + "GET_BOOL" + LPAREN + this->dispatch(elif.first) + RPAREN + RPAREN +
               LCURLY + NEWLINE + this->transpile_block(elif.second) + RCURLY;
    }
    if (in->_else != nullptr) {
        out += "else" + SPACE + LCURLY + NEWLINE + this->transpile_block(in->_else) + NEWLINE + RCURLY;
    }
    out += NEWLINE;
    return out;
}

std::string STranspiler::transpile_break(BreakSNode* bn) {
    std::string out = "break" + SEMIC + NEWLINE;
    return out;
}

std::string STranspiler::transpile_continue(ContinueSNode* pNode) {
    std::string out = "continue" + SEMIC + NEWLINE;
    return out;
}

std::string STranspiler::transpile_match(MatchSNode* mn) {
    std::string out;
    out += this->dispatch(mn->exp);
    out += "switch" + SPACE + LPAREN + "GET_INT(CAST(" + mn->varname + ",core_D_Union" + RPAREN + "->type" + RPAREN +
           RPAREN + SPACE + LCURLY;
    for (auto c: mn->cases) {
        out += "case" + SPACE + "" + std::to_string(c.first) + "" + SPACE + ":" + SPACE + LCURLY +
               this->transpile_block(c.second) + "break" + SEMIC + RCURLY;
    }
    out += RCURLY;
    return out;
}

void STranspiler::transpile_enum(EnumSNode* node) {
    std::string out;
    std::string enum_name = path_to_id(node->id);
    // out += "enum class" + SPACE + enum_name + SPACE + " {\n";
    // out += "";
    for (auto m: node->values) {
        out += "extern" + SPACE + TOBJECT + enum_name + "_" + m + SEMIC + NEWLINE;
    }
    this->header += out;
    int k = 0;
    out = "";
    for (auto m: node->values) {
        out += TOBJECT + enum_name + "_" + m + ASSIGN + "MAKE_INT(" + std::to_string(k) + ")" + SEMIC + NEWLINE;
        k++;
    }
    this->source += out;

    // out += RCURLY + SEMIC + NEWLINE;
    out = "";
    std::string eq_name = enum_name + "_D_eq";
    out += "extern" + SPACE + TOBJECT + eq_name + SEMIC;
    this->static_initializations += eq_name + SPACE + ASSIGN + SPACE + "core_D_Integer_D_eq" + SEMIC;

    this->source += TOBJECT + SPACE + eq_name + ASSIGN + "nullptr" + SEMIC + NEWLINE;

    std::string ne_name = enum_name + "_D_ne";
    out += "extern" + SPACE + TOBJECT + ne_name + SEMIC;
    this->static_initializations += ne_name + SPACE + ASSIGN + SPACE + "core_D_Integer_D_ne" + SEMIC;

    this->source += TOBJECT + SPACE + ne_name + ASSIGN + "nullptr" + SEMIC + NEWLINE;
    this->header += out;
}

std::string STranspiler::transpile_enum_member(EnumMemberSNode* emsn) {
    std::string out;
    out += path_to_id(emsn->enum_name) + "_" + emsn->value;
    return out;
}

std::string STranspiler::transpile_ternary(TernarySNode* tn) {
    std::string out;
    out += LPAREN + "(it=" + this->dispatch(tn->ext) + ")!=nullptr? " + this->dispatch(tn->true_case) + SPACE + ":" +
           SPACE + this->dispatch(tn->false_case) + RPAREN;
    return out;
}

std::string STranspiler::transpile_none(NoneSNode* nn) {
    return "nullptr";
}

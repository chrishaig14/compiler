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
    return out;
}

std::string STranspiler::transpile_return(ReturnSNode* node) {
    std::string out = TOBJECT + SPACE + RETURN_VAR + SPACE + ASSIGN + SPACE + GCRETURN + LPAREN + SPACE +
                      this->dispatch(node->expression) + RPAREN + SEMIC + NEWLINE;
    out += RETURN + SPACE + RETURN_VAR + SEMIC + NEWLINE;
    return out;
}

std::string STranspiler::transpile_id(IdSNode* node) {
    if (node->identifier == "") {
        throw std::runtime_error("Error: tranpiling empty idnode!");
    }
    std::string out;
    for (int i = 0; i < node->identifier.size(); i++) {
        char c = node->identifier[i];
        if (c == '.') {
            out += "_D_";
        } else {
            out += std::string(1, c);
        }
    }
    return out;
}

void STranspiler::transpile_function(FunctionSNode* node) {
    std::string parameters;
    for (auto pn: node->params) {
        std::string parameter = TOBJECT + SPACE + pn;
        parameters += parameter + COMMA + SPACE;
    }
    parameters = parameters.substr(0, parameters.size() - 2);

    std::string raw_function_identifier = node->identifier + "_f";

    std::string signature = TOBJECT + SPACE + raw_function_identifier + LPAREN + parameters + RPAREN;
    this->header += signature + SEMIC + NEWLINE;
    std::string f_source = signature + LCURLY + NEWLINE + this->transpile_block(node->body) + RCURLY + NEWLINE;

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
    out += "CALL" + std::to_string(node->arguments.size()) + "(" + this->dispatch(node->function) + ", " + arguments +
           ")";
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

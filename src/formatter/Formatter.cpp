//
// Created by chris on 26/8/20.
//

#include <iostream>
#include "Formatter.h"

void Formatter::visit(AssignmentNode& node) {
    int ind = this->indent_level;
    this->indent_level = 0;
    this->semicolon = false;
    node.lvalue->accept(*this);
    std::string lvalue = this->output;
    node.rvalue->accept(*this);
    std::string rvalue = this->output;
    this->indent_level = ind;
    this->output = this->indentation() + lvalue + " = " + rvalue + ";";

}

void Formatter::visit(BinopNode& node) {
    int ind = this->indent_level;
    this->indent_level = 0;
    node.left->accept(*this);
    std::string lvalue = this->output;
    node.right->accept(*this);
    std::string rvalue = this->output;
    std::string op;
    switch (node.op) {
        case OpType::ADD:
            op = "+";
            break;
        case OpType::SUB:
            op = "-";
            break;
        case OpType::MUL:
            op = "*";
            break;
        case OpType::DIV:
            op = "/";
            break;
        case OpType::EQ:
            op = "==";
            break;
        case OpType::AND:
            op = "&&";
            break;
        case OpType::OR:
            op = "||";
            break;
        case OpType::LEQ:
            op = "<=";
            break;
        case OpType::GEQ:
            op = ">=";
            break;
        case OpType::LT:
            op = "<";
            break;
        case OpType::GT:
            op = ">";
            break;
        case OpType::NEQ:
            op = "!=";
            break;
    }
    this->indent_level = ind;
    this->output = lvalue + " " + op + " " + rvalue;
}

void Formatter::visit(BreakNode& node) {
    this->output = this->indentation() + "break";
}

void Formatter::visit(BlockNode& node) {
    std::string all;
    this->semicolon = true;
    for (int i = 0; i < node.nodes.size(); i++) {
        node.nodes[i]->accept(*this);
        all += this->output;
        all += "\n";
    }
    this->output = all;
}

void Formatter::visit(CallNode& node) {
    int ind = this->indent_level;
    this->indent_level = 0;
    node.function->accept(*this);
    std::string function = this->output;
    std::string arguments;
    for (int i = 0; i < node.arguments.size(); i++) {
        node.arguments[i]->accept(*this);
        arguments += this->output + ", ";
    }
    arguments = arguments.substr(0, arguments.size() - 2);
    this->indent_level = ind;
    this->output = this->indentation() + function + "(" + arguments + ")";
    if (this->semicolon) {
        this->output += ';';
    }
}

void Formatter::visit(ClassLiteralExpressionNode& node) {

}

void Formatter::visit(ClassLiteralFieldNode& node) {

}

void Formatter::visit(DeclarationNode& node) {
    int ind = this->indent_level;
    this->indent_level = 0;
    this->semicolon = false;
    node.expression->accept(*this);
    std::string expression = this->output;
    std::string type;
    this->indent_level = ind;
    std::cout << "the indentation level here is" << this->indent_level;
    this->output = this->indentation() + "var " + node.identifier;
    if (node.type != nullptr) {
        this->output += ":" + node.type->to_string();
    }
    this->output += " = " + expression + ";";
}

void Formatter::visit(ForNode& node) {
    throw std::runtime_error("formatting for ForNode not implemented!");
}

void Formatter::visit(FunctionNode& node) {
    std::string parameters;
    for (int i = 0; i < node.parameter_names.size(); i++) {
        std::string parameter_name = node.parameter_names[i];
        std::string parameter_type = node.parameter_types[i]->to_string();
        parameters += parameter_name + " : " + parameter_type + ", ";
    }
    std::string return_type = node.return_type->to_string();
    this->indent_level++;
    std::cout << "formatting body of function at indent level " << this->indent_level << std::endl;
    node.body->accept(*this);
    this->indent_level--;
    std::string body = this->output;

    this->output = "fun " + node.identifier + "(" + parameters + ") -> " + return_type + " {\n" + body + "}\n";
}

void Formatter::visit(BooleanNode& node) {
    this->output = node.value ? "true" : "false";
}

void Formatter::visit(IdNode& node) {
    this->output = node.identifier;
}

void Formatter::visit(IfNode& node) {
    node.condition->accept(*this);
    int ind = this->indent_level;
    this->indent_level = 0;
    std::string condition = this->output;
    this->indent_level = ind;
    this->indent_level++;
    node.then->accept(*this);
    this->indent_level--;
    std::string then = this->output;
    this->output = this->indentation() + "if (" + condition + "){\n" + then + this->indentation() + "}";
}

void Formatter::visit(ListNode& node) {
    std::string elements;
    for (int i = 0; i < node.elements.size(); i++) {
        Node* element = node.elements[i];
        element->accept(*this);
        std::string element_str = this->output;
        elements += element_str + ", ";
    }
    this->output = "[" + elements + "]";
}

void Formatter::visit(MemberNode& node) {
    node.parent->accept(*this);
    std::string parent = this->output;
    this->output = parent + "." + node.child;
}

void Formatter::visit(NumberNode& node) {
    this->output = std::to_string(node.number);
}

void Formatter::visit(ReturnNode& node) {
    int ind = this->indent_level;
    this->indent_level = 0;
    this->semicolon = false;
    node.expression->accept(*this);
    this->indent_level = ind;
    std::string expression = this->output;
    this->output = this->indentation() + "return " + expression + ";";
}

void Formatter::visit(StringNode& node) {
    this->output = "\"" + node.str + "\"";
}

void Formatter::visit(StructNode& node) {
    std::string fields;
    for (int i = 0; i < node.fields.size(); i++) {
        FieldInfo field = node.fields[i];
        field.second->accept(*this);
        std::string field_type = this->output;
        fields += "\t" + field.first + ": " + field_type + ";\n";
    }
    this->output = "struct " + node.identifier + "{\n" + fields + "}\n";
}

void Formatter::visit(SubscriptNode& node) {
    node.parent->accept(*this);
    std::string parent = this->output;
    node.child->accept(*this);
    std::string child = this->output;
    this->output = parent + "[" + child + "]";
}

void Formatter::visit(TypeNode& node) {
    this->output = node.to_string();
}

void Formatter::visit(WhileNode& node) {
    node.condition->accept(*this);
    std::string condition = this->output;
    this->indent_level++;
    std::cout << "formatting body of while at indent level" << this->indent_level << std::endl;
    node.body->accept(*this);
    this->indent_level--;
    std::string body = this->output;
    this->output = this->indentation() + "while (" + condition + ") {\n" + body + this->indentation() + "}";
}

void Formatter::visit(TernaryNode& node) {

}

//
// Created by chris on 25/8/21.
//

#ifndef XLANG_AST_H
#define XLANG_AST_H

#include <memory>
#include <vector>

namespace ast {
    class Module;

    class Continue;

    class CallExp;

    class Break;

    class DefaultConstructor;

    class EmptyDict;

    class EmptyList;

    class EnumNode;

    class Import;

    class List;

    class Instance;

    class Match;

    class Number;

    class None;

    class Subscript;

    class Tuple;

    class UnaryOp;

    class BoolOp;

    class Cast;

    class Ternary;

    class PartialApplication;

    class DictNode;

    class CommonNode;

    class Typeclass;

    class Alias;

    class BinaryOp;

    class Declaration;

    class Id;

    class Call;

    class Klass;

    class Block;

    class While;

    class Function;

    class Member;

    class Assignment;

    class Boolean;

    class Return;

    class String;

    class If;

    class For;

    class Type;

    class ObjectType;

    class TopNode;

    class ExpNode;

    class FunctionType;

    typedef std::unique_ptr<Function> UFunctionNode;
    typedef std::unique_ptr<BinaryOp> UBinaryOp;
    typedef std::unique_ptr<Id> UId;
    typedef std::unique_ptr<Block> UBlock;
    typedef std::unique_ptr<Number> UNumber;
    typedef std::unique_ptr<CommonNode> UNode;
    typedef std::unique_ptr<ExpNode> UExpNode;
    typedef std::reference_wrapper<CommonNode> RNode;
    typedef std::reference_wrapper<ExpNode> RExpNode;
    typedef std::vector<UNode> VectorOfNodesU;
    typedef std::vector<UExpNode> VectorOfExpNodesU;

    typedef std::unique_ptr<ObjectType> UObjectType;
    typedef std::unique_ptr<FunctionType> UFunctionType;
    typedef std::unique_ptr<Type> UTypeNode;
    typedef std::vector<UTypeNode> VectorOfUTypes;
    typedef std::vector<ast::Type*> VectorOfTypes;

}

#endif //XLANG_AST_H

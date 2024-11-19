#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

#include "token.h"
#include <QVector>

namespace ExpressionNode{
    typedef double (*FuncPtr)(double);

    struct ExprNode {
        Token::TokenType op_code;
        union {
            struct {
                ExprNode *left, *right;
            } case_operator;
            struct {
                ExprNode *child;
                FuncPtr math_func_ptr;
            } case_func;
            double case_const;
            double *case_parm_ptr;
        } content;
    };
}

#endif // EXPRESSIONNODE_H

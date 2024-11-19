#ifndef PARSERS_H
#define PARSERS_H

#include <QObject>
#include "expressionnode.h"

#include <QDebug>

using namespace ExpressionNode;
using namespace Token;

class Parsers : public QObject
{
    Q_OBJECT
public:
    explicit Parsers(const QVector<Tokens> &token_stream, QObject *parent = nullptr);
    Parsers(const QVector<Tokens> &token_stream);
    void outExprNode(struct ExprNode* tree);
    void fetchToken();
    void matchToken(TokenType t);
    void syntaxError(int x);
    double getExpValue(struct ExprNode* tree);
    void parser();
    void program();
    void statement();
    void notesStatement();
    void colorStatement();
    void originStatement();
    void rotStatement();
    void scaleStatement();
    void forStatement();

public:
    struct ExprNode* makeExprNode(TokenType op_code, ...) {
        struct ExprNode *expr_ptr = new (struct ExprNode);
        expr_ptr->op_code = op_code;
        va_list arg_ptr;
        va_start(arg_ptr, op_code);
        switch (op_code) {
        case CONST_ID: expr_ptr->content.case_const = (double)va_arg(arg_ptr, double); break;
        case T: expr_ptr->content.case_parm_ptr = &parameter; break;
        case FUNC:
            expr_ptr->content.case_func.math_func_ptr = (FuncPtr)va_arg(arg_ptr, FuncPtr);
            expr_ptr->content.case_func.child = (struct ExprNode*)va_arg(arg_ptr, struct ExprNode*); break;
        default:
            expr_ptr->content.case_operator.left = (struct ExprNode*)va_arg(arg_ptr, struct ExprNode*);
            expr_ptr->content.case_operator.right = (struct ExprNode*)va_arg(arg_ptr, struct ExprNode*); break;
        }
        va_end(arg_ptr);
        return expr_ptr;
    }

    struct ExprNode* expression() {
        struct ExprNode *left, *right;
        TokenType token_tmp;
        left = term();
        while (temp_token.type == PLUS || temp_token.type == MINUS) {
            token_tmp = temp_token.type;
            matchToken(token_tmp);
            right = term();
            left = makeExprNode(token_tmp, left, right);
        }
        return left;
    }

    struct ExprNode* term() {
        struct ExprNode *left, *right;
        TokenType token_tmp;
        left = factor();
        while (temp_token.type == MUL || temp_token.type == DIV) {
            token_tmp = temp_token.type;
            matchToken(token_tmp);
            right = factor();
            left = makeExprNode(token_tmp, left, right);
        }
        return left;
    }

    struct ExprNode* factor() {
        struct ExprNode *left, *right;
        if (temp_token.type == PLUS) {
            matchToken(PLUS);
            right = factor();
            left = nullptr;
            right = makeExprNode(PLUS, left, right);
        } else if (temp_token.type == MINUS) {
            matchToken(MINUS);
            right = factor();
            left = makeExprNode(CONST_ID, 0.0);
            right = makeExprNode(MINUS, left, right);
        } else right = component();
        return right;
    }

    struct ExprNode* component() {
        struct ExprNode *left, *right;
        left = atom();
        if (temp_token.type == POWER) {
            matchToken(POWER);
            right = component();
            left = makeExprNode(POWER, left, right);
        }
        return left;
    }

    struct ExprNode* atom() {
        struct ExprNode *address, *tmp;
        double const_value;
        FuncPtr func_ptr_value;
        if (temp_token.type == CONST_ID) {
            const_value = temp_token.value;
            matchToken(CONST_ID);
            address = makeExprNode(CONST_ID, const_value);
        } else if (temp_token.type == T) {
            matchToken(T);
            address = makeExprNode(T);
        } else if (temp_token.type == FUNC) {
            func_ptr_value = temp_token.FuncPtr;
            matchToken(FUNC);
            matchToken(L_BRACKET);
            tmp = expression();
            address = makeExprNode(FUNC, func_ptr_value, tmp);
            matchToken(R_BRACKET);
        } else if (temp_token.type == L_BRACKET) {
            matchToken(L_BRACKET);
            address = expression();
            matchToken(R_BRACKET);
        } else syntaxError(2);
        return address;
    }

public:
    struct ExprNode *origin_x_ptr, *origin_y_ptr, *scale_x_ptr, *scale_y_ptr, *rot_ptr, *start_ptr, *end_ptr, *step_ptr, *for_x_ptr, *for_y_ptr, *color_r_ptr, *color_g_ptr, *color_b_ptr, *notes_x_ptr, *notes_y_ptr;
    static int cnt;
    double origin_x = 0, origin_y = 0, scale_x = 1, scale_y = 1, rot_ang = 0, r = 255, g = 255, b = 255, x_notes, y_notes;
    Tokens temp_token;
    QVector<Tokens> token_stream;
    double parameter = 0;
    QString temp_notes = "";
    QVector<double> r_values;
    QVector<double> g_values;
    QVector<double> b_values;
    QVector<double> origin_x_values;
    QVector<double> origin_y_values;
    QVector<double> scale_x_values;
    QVector<double> scale_y_values;
    QVector<double> rot_ang_values;
    QVector<double> start_values;
    QVector<double> end_values;
    QVector<double> step_values;
    QVector<struct ExprNode*> for_x_values;
    QVector<struct ExprNode*> for_y_values;
    QVector<double> notes_x_values;
    QVector<double> notes_y_values;
    QVector<QString> notes_string_values;
    QVector<double> notes_r_values;
    QVector<double> notes_g_values;
    QVector<double> notes_b_values;
signals:
    void parsersOutputSent(const QString &text, Qt::GlobalColor color);
};

#endif // PARSERS_H

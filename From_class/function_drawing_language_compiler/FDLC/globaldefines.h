#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#include <QVector>
#include <QString>

#define PI M_PI
#define E M_E

enum TokenType {
    ORIGIN, SCALE, ROT, IS, TO, STEP, DRAW, FOR, FROM, COLOR, NOTES, //保留字
    T, //参数
    SEMICO, L_BRACKET, R_BRACKET, COMMA, COLON, QUOTES, //; ( ) ,分隔符
    PLUS, MINUS, MUL, DIV, POWER,  //+ - * / **运算符
    FUNC, //函数
    CONST_ID, //常数
    NONTOKEN, //空记号
    ERRTOKEN //出错记号
};

typedef struct Tokens {
    TokenType type;
    QString lexeme;
    double value;
    double (*FuncPtr)(double);
    int token_line;
} Tokens;

static Tokens TOKEN_TABLE[] = {
    {ORIGIN, "ORIGIN", 0.0, nullptr, 0},
    {COLOR, "COLOR", 0.0, nullptr, 0},
    {NOTES, "NOTES", 0.0, nullptr, 0},
    {SCALE, "SCALE", 0.0, nullptr, 0},
    {ROT, "ROT", 0.0, nullptr, 0},
    {IS, "IS", 0.0, nullptr, 0},
    {TO, "TO", 0.0, nullptr, 0},
    {STEP, "STEP", 0.0, nullptr, 0},
    {DRAW, "DRAW", 0.0, nullptr, 0},
    {FOR, "FOR", 0.0, nullptr, 0},
    {FROM, "FROM", 0.0, nullptr, 0},
    {T, "T", 0.0, nullptr, 0},
    {FUNC, "SIN", 0.0, sin, 0},
    {FUNC, "COS", 0.0, cos, 0},
    {FUNC, "TAN", 0.0, tan, 0},
    {FUNC, "LN", 0.0, log, 0},
    {FUNC, "EXP", 0.0, exp, 0},
    {FUNC, "SQRT", 0.0, sqrt, 0},
    {CONST_ID, "PI", PI, nullptr, 0},
    {CONST_ID, "E", E, nullptr, 0}
};

extern QVector<Tokens> token_stream;
extern void loadTokensFromString(const QString &input);

typedef double (*FuncPtr)(double);
struct ExprNode {
    TokenType op_code;
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

struct DrawNode{
    double x;
    double y;
    double r;
    double g;
    double b;
};

struct DrawText{
    DrawNode set;
    QString str;
};

extern QVector<double> r_values;
extern QVector<double> g_values;
extern QVector<double> b_values;
extern QVector<double> origin_x_values;
extern QVector<double> origin_y_values;
extern QVector<double> scale_x_values;
extern QVector<double> scale_y_values;
extern QVector<double> rot_ang_values;
extern QVector<double> start_values;
extern QVector<double> end_values;
extern QVector<double> step_values;
extern QVector<struct ExprNode*> for_x_values;
extern QVector<struct ExprNode*> for_y_values;
extern QVector<double> notes_x_values;
extern QVector<double> notes_y_values;
extern QVector<QString> notes_string_values;
extern QVector<double> notes_r_values;
extern QVector<double> notes_g_values;
extern QVector<double> notes_b_values;

extern double parameter;
extern QString temp_notes;

extern QVector<struct DrawNode> draw_node;
extern QVector<struct DrawText> draw_text;

void initAll();

#endif // GLOBALDEFINES_H

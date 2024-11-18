#ifndef SCANNER_H
#define SCANNER_H

#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QChar>
#include <QVector>
#include <QtMath>

#define PI M_PI
#define E M_E

enum TokenType {
    ORIGIN, SCALE, ROT, IS, TO, STEP, DRAW, FOR, FROM, // 保留字
    T, // 参数
    SEMICO, L_BRACKET, R_BRACKET, COMMA, // ; ( ) ,分隔符
    PLUS, MINUS, MUL, DIV, POWER,  // + - * / **运算符
    FUNC, // 函数
    CONST_ID, // 常数
    NONTOKEN, // 空记号
    ERRTOKEN // 出错记号
};

typedef struct Tokens {
    TokenType type;
    QString lexeme;
    double value;
    double (*func_ptr)(double);
    int token_line;
} Tokens;

static Tokens TOKEN_TAB[] = {
    {ORIGIN, "ORIGIN", 0.0, nullptr, 0},
    {SCALE, "SCALE", 0.0, nullptr, 0},
    {ROT, "ROT", 0.0, nullptr, 0},
    {IS, "IS", 0.0, nullptr, 0},
    {TO, "TO", 0.0, nullptr, 0},
    {STEP, "STEP", 0.0, nullptr, 0},
    {DRAW, "DRAW", 0.0, nullptr, 0},
    {FOR, "FOR", 0.0, nullptr, 0},
    {FROM, "FROM", 0.0, nullptr, 0},
    {T, "T", 0.0, nullptr, 0},
    {FUNC, "SIN", 0.0, qSin, 0},
    {FUNC, "COS", 0.0, qCos, 0},
    {FUNC, "TAN", 0.0, qTan, 0},
    {FUNC, "LN", 0.0, qLn, 0},
    {FUNC, "EXP", 0.0, qExp, 0},
    {FUNC, "SQRT", 0.0, qSqrt, 0},
    {CONST_ID, "PI", PI, nullptr, 0},
    {CONST_ID, "E", E, nullptr, 0}
};

class Scanner {
public:
    Scanner(const QString &codes) : codes(codes), position(0) {}
    void emptyBuffer();
    void addCharToBuffer(QChar temp_c);
    Tokens searchCharInDict(QString temp_s);
    Tokens createToken(TokenType type, const QString &lexeme, double value, double (*func_ptr)(double), int line);
    Tokens getToken();

private:
    QString codes;
    QString token_buffer;
    int position;
};
#endif // SCANNER_H

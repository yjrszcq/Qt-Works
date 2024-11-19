#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QtMath>

#define PI M_PI
#define E M_E

namespace Token{
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
}

#endif // TOKEN_H

#include "scanner.h"

Tokens Scanner::TOKEN_TABLE[] = {
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

void Scanner::emptyBuffer() {
    token_buffer.clear();
}

void Scanner::addCharToBuffer(QChar temp_c) {
    token_buffer.append(temp_c);
}

Tokens Scanner::searchCharInDict(const QString &temp_s) {
    Tokens t = {ERRTOKEN, temp_s, 0.0, nullptr, 0};
    QString upper_temp_s = temp_s.toUpper();
    for (int i = 0; i < 20; ++i) {
        if (upper_temp_s == TOKEN_TABLE[i].lexeme) {
            t.type = TOKEN_TABLE[i].type;
            t.value = TOKEN_TABLE[i].value;
            t.FuncPtr = TOKEN_TABLE[i].FuncPtr;
        }
    }
    return t;
}

Tokens Scanner::createToken(TokenType type, const QString &lexeme, double value, double (*FuncPtr)(double), int line) {
    Tokens temp_token;
    temp_token.type = type;
    temp_token.lexeme = lexeme;
    temp_token.value = value;
    temp_token.FuncPtr = FuncPtr;
    temp_token.token_line = line;
    return temp_token;
}

Tokens Scanner::getToken() {
    QChar c;
    static int line = 1; //行数
    Tokens temp_t;
    emptyBuffer();   //清空缓冲区
    while (1) {
        c = stream.read(1)[0];
        if (c.isNull()) {
            temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
            return temp_t;
        } else if (c.isSpace()) continue;
        else if (c == '\n') { line++; continue; }
        else if (c.isLetter()) {
            addCharToBuffer(c);
            while (1) {
                c = stream.read(1)[0];
                if (c.isLetterOrNumber()) addCharToBuffer(c);
                else break;
            }
            stream.seek(stream.pos() - 1);
            temp_t = searchCharInDict(token_buffer);
            temp_t.token_line = line;
            return temp_t;
        } else if (c.isDigit()) {
            addCharToBuffer(c);
            while (1) {
                c = stream.read(1)[0];
                if (c.isDigit()) addCharToBuffer(c);
                else break;
            }
            if (c == '.') {
                addCharToBuffer(c);
                while (1) {
                    c = stream.read(1)[0];
                    if (c.isDigit()) addCharToBuffer(c);
                    else break;
                }
            }
            stream.seek(stream.pos() - 1);
            temp_t = createToken(CONST_ID, token_buffer, token_buffer.toDouble(), nullptr, line);
            return temp_t;
        } else if (c == ';') {
            temp_t = createToken(SEMICO, ";", 0.0, nullptr, line);
            return temp_t;
        } else if (c == '\"') {
            temp_t = createToken(QUOTES, "\"", 0.0, nullptr, line);
            return temp_t;
        } else if (c == ':') {
            temp_t = createToken(COLON, ":", 0.0, nullptr, line);
            return temp_t;
        } else if (c == '(') {
            temp_t = createToken(L_BRACKET, "(", 0.0, nullptr, line);
            return temp_t;
        } else if (c == ')') {
            temp_t = createToken(R_BRACKET, ")", 0.0, nullptr, line);
            return temp_t;
        } else if (c == ',') {
            temp_t = createToken(COMMA, ",", 0.0, nullptr, line);
            return temp_t;
        } else if (c == '+') {
            temp_t = createToken(PLUS, "+", 0.0, nullptr, line);
            return temp_t;
        } else if (c == '-') {
            c = stream.read(1)[0];
            if (c == '-') {
                while (c != '\n' && !stream.atEnd()) c = stream.read(1)[0];
                if (stream.atEnd()) {
                    temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
                    return temp_t;
                } else {
                    line++;
                    continue;
                }
            } else {
                stream.seek(stream.pos() - 1);
                temp_t = createToken(MINUS, "-", 0.0, nullptr, line);
                return temp_t;
            }
        } else if (c == '/') {
            c = stream.read(1)[0];
            if (c == '/') {
                while (c != '\n' && !stream.atEnd()) c = stream.read(1)[0];
                if (stream.atEnd()) {
                    temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
                    return temp_t;
                } else {
                    line++;
                    continue;
                }
            } else {
                stream.seek(stream.pos() - 1);
                temp_t = createToken(DIV, "/", 0.0, nullptr, line);
                return temp_t;
            }
        } else if (c == '*') {
            c = stream.read(1)[0];
            if (c == '*') {
                temp_t = createToken(POWER, "**", 0.0, nullptr, line);
                return temp_t;
            } else {
                stream.seek(stream.pos() - 1);
                temp_t = createToken(MUL, "*", 0.0, nullptr, line);
                return temp_t;
            }
        } else {
            temp_t = createToken(ERRTOKEN, "EOF", 0.0, nullptr, line);
            return temp_t;
        }
    }
}

#include "scanner.h"

void Scanner::emptyBuffer() { // 清空缓冲区
    token_buffer.clear();
}

void Scanner::addCharToBuffer(QChar temp_c) { // 将字符添加到缓冲区
    token_buffer.append(temp_c);
}

Tokens Scanner::searchCharInDict(QString temp_s) { // 查字典
    Tokens t = {ERRTOKEN, temp_s, 0.0, nullptr, 0};
    temp_s = temp_s.toUpper();
    for (int i = 0; i < 18; i++) {
        if (temp_s == TOKEN_TAB[i].lexeme) {
            t.type = TOKEN_TAB[i].type;
            t.value = TOKEN_TAB[i].value;
            t.func_ptr = TOKEN_TAB[i].func_ptr;
        }
    }
    return t;
}

Tokens Scanner::createToken(TokenType type, const QString &lexeme, double value, double (*func_ptr)(double), int line) {
    Tokens temp_token;
    temp_token.type = type;
    temp_token.lexeme = lexeme;
    temp_token.value = value;
    temp_token.func_ptr = func_ptr;
    temp_token.token_line = line;
    return temp_token;
}

Tokens Scanner::getToken() {
    QChar c;
    static int line = 1; // 行数
    Tokens temp_t;
    emptyBuffer();   // 清空缓冲区
    while (position < codes.size()) {
        c = codes.at(position++);
        if (c == ' ' || c == '\t') continue;
        else if (c == '\n') { line++; continue; }
        else if (c.isLetter()) {
            addCharToBuffer(c);
            while (position < codes.size()) {
                c = codes.at(position);
                if (c.isLetterOrNumber()) {
                    addCharToBuffer(c);
                    position++;
                } else break;
            }
            temp_t = searchCharInDict(token_buffer);
            temp_t.token_line = line;
            return temp_t;
        } else if (c.isDigit()) {
            addCharToBuffer(c);
            while (position < codes.size()) {
                c = codes.at(position);
                if (c.isDigit()) {
                    addCharToBuffer(c);
                    position++;
                } else break;
            }
            if (position < codes.size() && codes.at(position) == '.') {
                addCharToBuffer(codes.at(position++));
                while (position < codes.size()) {
                    c = codes.at(position);
                    if (c.isDigit()) {
                        addCharToBuffer(c);
                        position++;
                    } else break;
                }
            }
            temp_t = createToken(CONST_ID, token_buffer, token_buffer.toDouble(), nullptr, line);
            return temp_t;
        } else if (c == ';') {
            temp_t = createToken(SEMICO, ";", 0.0, nullptr, line);
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
            if (position < codes.size() && codes.at(position) == '-') {
                while (position < codes.size() && codes.at(position) != '\n') {
                    position++;
                }
                if (position < codes.size()) {
                    line++;
                    continue;
                } else {
                    temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
                    return temp_t;
                }
            } else {
                temp_t = createToken(MINUS, "-", 0.0, nullptr, line);
                return temp_t;
            }
        } else if (c == '/') {
            if (position < codes.size() && codes.at(position) == '/') {
                while (position < codes.size() && codes.at(position) != '\n') {
                    position++;
                }
                if (position < codes.size()) {
                    line++;
                    continue;
                } else {
                    temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
                    return temp_t;
                }
            } else {
                temp_t = createToken(DIV, "/", 0.0, nullptr, line);
                return temp_t;
            }
        } else if (c == '*') {
            if (position < codes.size() && codes.at(position) == '*') {
                temp_t = createToken(POWER, "**", 0.0, nullptr, line);
                position++;
                return temp_t;
            } else {
                temp_t = createToken(MUL, "*", 0.0, nullptr, line);
                return temp_t;
            }
        } else {
            temp_t = createToken(ERRTOKEN, "EOF", 0.0, nullptr, line);
            return temp_t;
        }
    }
    temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
    return temp_t;
}

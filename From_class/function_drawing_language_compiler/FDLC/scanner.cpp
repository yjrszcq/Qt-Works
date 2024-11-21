#include "scanner.h"

Scanner::Scanner(const QString &codes, QObject *parent)
    : QObject{parent}
{
    this->codes = codes;
    stream.setString(&this->codes);
}

void Scanner::emptyBuffer() {
    token_buffer.clear();
}

void Scanner::addCharToBuffer(QChar temp_c) {
    token_buffer.append(temp_c);
}

Tokens Scanner::searchCharInDict(const QString &temp_s) {
    Tokens t = {ERRTOKEN, temp_s, 0.0, nullptr, 0};
    try{
        QString upper_temp_s = temp_s.toUpper();
        bool jud = false;
        for (int i = 0; i < 20; ++i) {
            if (upper_temp_s == TOKEN_TABLE[i].lexeme) {
                t.type = TOKEN_TABLE[i].type;
                t.value = TOKEN_TABLE[i].value;
                t.FuncPtr = TOKEN_TABLE[i].FuncPtr;
                jud = true;
            }
        }
        if(jud == false){
            throw std::runtime_error(("Unkonwn Token: " + temp_s).toStdString());
        }
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
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
    emit scannerStatusSent(S_START);
    QChar c;
    static int line = 1; //行数
    Tokens temp_t;
    emptyBuffer();   //清空缓冲区
    try{
        while (1) {
            c = stream.read(1).at(0);
            if (c.isNull()) {
                temp_t = createToken(NONTOKEN, "EOF", 0.0, nullptr, line);
                return temp_t;
            } else if (c.isSpace()) continue;
            else if (c == '\n') { line++; continue; }
            else if (c.isLetter()) {
                addCharToBuffer(c);
                while (1) {
                    c = stream.read(1).at(0);
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
                    c = stream.read(1).at(0);
                    if (c.isDigit()) addCharToBuffer(c);
                    else break;
                }
                if (c == '.') {
                    addCharToBuffer(c);
                    while (1) {
                        c = stream.read(1).at(0);
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
                c = stream.read(1).at(0);
                if (c == '-') {
                    while (c != '\n' && !stream.atEnd()) c = stream.read(1).at(0);
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
                c = stream.read(1).at(0);
                if (c == '/') {
                    while (c != '\n' && !stream.atEnd()) c = stream.read(1).at(0);
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
                c = stream.read(1).at(0);
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
        emit scannerStatusSent(S_SUCCEED);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
}

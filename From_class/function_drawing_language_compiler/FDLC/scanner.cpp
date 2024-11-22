#include "scanner.h"
#include <QDebug>

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
    tokenOutputType(t);
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

void Scanner::tokenOutputType(Tokens token) {
    switch (token.type) {
    case ORIGIN: tokenOutput("ORIGIN", token.lexeme, token.value, "NULL"); break;
    case SCALE: tokenOutput("SCALE", token.lexeme, token.value, "NULL"); break;
    case ROT: tokenOutput("ROT", token.lexeme, token.value, "NULL"); break;
    case IS: tokenOutput("IS", token.lexeme, token.value, "NULL"); break;
    case TO: tokenOutput("TO", token.lexeme, token.value, "NULL"); break;
    case STEP: tokenOutput("STEP", token.lexeme, token.value, "NULL"); break;
    case DRAW: tokenOutput("DRAW", token.lexeme, token.value, "NULL"); break;
    case FOR: tokenOutput("FOR", token.lexeme, token.value, "NULL"); break;
    case FROM: tokenOutput("FROM", token.lexeme, token.value, "NULL"); break;
    case T: tokenOutput("T", token.lexeme, token.value, "NULL"); break;
    case SEMICO: tokenOutput("SEMICO", token.lexeme, token.value, "NULL"); break;
    case L_BRACKET: tokenOutput("L_BRACKET", token.lexeme, token.value, "NULL"); break;
    case R_BRACKET: tokenOutput("R_BRACKET", token.lexeme, token.value, "NULL"); break;
    case COMMA: tokenOutput("COMMA", token.lexeme, token.value, "NULL"); break;
    case PLUS: tokenOutput("PLUS", token.lexeme, token.value, "NULL"); break;
    case MINUS: tokenOutput("MINUS", token.lexeme, token.value, "NULL"); break;
    case MUL: tokenOutput("MUL", token.lexeme, token.value, "NULL"); break;
    case DIV: tokenOutput("DIV", token.lexeme, token.value, "NULL"); break;
    case POWER: tokenOutput("POWER", token.lexeme, token.value, "NULL"); break;
    case FUNC: tokenOutput("FUNC", token.lexeme, token.value, token.lexeme.toUpper()); break;
    case CONST_ID: tokenOutput("CONST_ID", token.lexeme, token.value, "NULL"); break;
    case NONTOKEN: tokenOutput("NONTOKEN", token.lexeme, token.value, "NULL"); break;
    case ERRTOKEN: tokenOutput("ERRTOKEN", token.lexeme, token.value, "NULL"); break;
    case COLOR: tokenOutput("COLOR", token.lexeme, token.value, "NULL"); break;
    case NOTES: tokenOutput("NOTES", token.lexeme, token.value, "NULL"); break;
    case COLON: tokenOutput("COLON", token.lexeme, token.value, "NULL"); break;
    case QUOTES: tokenOutput("QUOTES", token.lexeme, token.value, "NULL"); break;
    }
}

void Scanner::tokenOutput(const QString &type, const QString &lexeme, double value, const QString &func_ptr) {
    QString output = QString("%1\t|\t%2\t|\t%3\t|\t%4\n").arg(type, 10).arg(lexeme, 12).arg(value, 14, 'f', 6).arg(func_ptr, 16);
    if(type != "ERRTOKEN"){
        emit scannerOutputSent(output, Qt::black, lexeme);
    } else{
        emit scannerOutputSent(output, Qt::red, lexeme);
    }
}

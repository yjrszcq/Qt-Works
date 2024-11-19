#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include <QTextStream>
#include <QChar>
#include <QVector>

using namespace Token;

class Scanner {
public:
    Scanner(const QString &codes){
        this->codes = codes;
        stream.setString(&this->codes);
    }
    void emptyBuffer();
    void addCharToBuffer(QChar temp_c);
    Tokens searchCharInDict(const QString &temp_s);
    Tokens createToken(TokenType type, const QString &lexeme, double value, double (*FuncPtr)(double), int line);
    Tokens getToken();

private:
    QString token_buffer; //缓冲区
    QString codes;
    QTextStream stream;
    static Tokens TOKEN_TABLE[];
};
#endif // SCANNER_H

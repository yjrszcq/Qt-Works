#ifndef SCANNER_H
#define SCANNER_H

#include "globaldefines.h"
#include <QTextStream>
#include <QChar>
#include <QVector>

class Scanner {
public:
    Scanner(const QString &codes);
    void emptyBuffer();
    void addCharToBuffer(QChar temp_c);
    Tokens searchCharInDict(const QString &temp_s);
    Tokens createToken(TokenType type, const QString &lexeme, double value, double (*FuncPtr)(double), int line);
    Tokens getToken();

private:
    QString token_buffer; //缓冲区
    QString codes;
    QTextStream stream;
};
#endif // SCANNER_H

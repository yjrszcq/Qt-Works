#ifndef SCANNER_H
#define SCANNER_H

#include <QObject>
#include "globaldefines.h"
#include <QTextStream>

class Scanner : public QObject
{
    Q_OBJECT
public:
    enum Status{S_PREPARING, S_START, S_SUCCEED};
    explicit Scanner(const QString &codes, QObject *parent = nullptr);
    void emptyBuffer();
    void addCharToBuffer(QChar temp_c);
    Tokens searchCharInDict(const QString &temp_s);
    Tokens createToken(TokenType type, const QString &lexeme, double value, double (*FuncPtr)(double), int line);
    Tokens getToken();

private:
    QString token_buffer; //缓冲区
    QString codes;
    QTextStream stream;

signals:
    void scannerStatusSent(Scanner::Status status);
};

#endif // SCANNER_H

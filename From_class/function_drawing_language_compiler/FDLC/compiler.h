#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include "scanner.h"
#include "parsers.h"

#include <QUrl>
#include <QMutex>

class Compiler : public QObject
{
    Q_OBJECT
public:
    enum Status{C_PREPARING, C_START, C_SCANNING, C_PARSERING, C_SUCCEED, C_FAILED};
    explicit Compiler(QString codes, QObject *parent = nullptr);
    static bool run(QString codes);

public:
    void callScanner();
    void callParsers();
    void outputScannerError();
    static void outputNodeData(QUrl file_path);

private:
    QString codes;
    QMutex mutex;
    Status status;
    Scanner::Status scanner_status;
    Parsers::Status parsers_status;
    QString error_tokens_str;

public slots:
    bool compile();
    void scannerStatusReceive(Scanner::Status scanner_status);
    void scannerOutputReceive(const QString &text, Qt::GlobalColor color, const QString token_lexeme);
    void parsersStatusReceive(Parsers::Status parsers_status);
    void parsersOutputReceive(const QString &text, Qt::GlobalColor color);

signals:
    void clearSent();
    void resultSent();
    void processSent(QString process, Qt::GlobalColor color);
    void errorSent(QString error, Status status, Scanner::Status scanner_status, Parsers::Status parsers_status);
};

#endif // COMPILER_H

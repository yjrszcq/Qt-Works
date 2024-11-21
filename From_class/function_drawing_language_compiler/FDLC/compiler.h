#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include "globaldefines.h"
#include "scanner.h"
#include "parsers.h"

#include <QUrl>
#include <QMutex>

class Compiler : public QObject
{
    Q_OBJECT
public:
    enum Status{PREPARING, START, SCANNING, PARSERING, SUCCEED, FAILED};
    explicit Compiler(QString codes, QUrl file_path, QObject *parent = nullptr);
    static bool run(QString codes, QUrl file_path);

public:
    void callScanner();
    void callParsers();
    void sts(const QString &type, const QString &lexeme, double value, const QString &func_ptr);
    void stsOut(Tokens token);
    void outputScannerResult();
    void nodeTotalXY(QTextStream &out_total, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y);
    void nodeXY(QTextStream &out_total, double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b);
    void outTextXY(QTextStream &out_text, double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b);
    void outputParsersResult();

private:
    QString codes;
    QUrl file_path;
    QMutex mutex;
    Status status;
    Scanner::Status scanner_status;
    Parsers::Status parsers_status;

public slots:
    bool compile();
    void scannerStatusReceive(Scanner::Status scanner_status);
    void parsersStatusReceive(Parsers::Status parsers_status);
    void parsersOutputReceive(const QString &text, Qt::GlobalColor color);

signals:
    void refreshSent();
    void resultSent();
    void processSent(QString process, Qt::GlobalColor color);
    void errorSent(QString error, Status status, Scanner::Status scanner_status, Parsers::Status parsers_status);
};

#endif // COMPILER_H

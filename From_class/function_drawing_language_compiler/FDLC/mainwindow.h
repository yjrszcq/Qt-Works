#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "codeeditor.h"
#include "token.h"
#include "parsers.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QTextEdit>

using namespace Token;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_new_file_triggered();
    void on_open_file_triggered();

    void on_save_flie_triggered();

    void on_font_triggered();

    void on_compile_triggered();

    void on_color_triggered();

private:
    Ui::MainWindow *ui;
    CodeEditor *ce;

public:
    enum SaveType {TEXT, CODE, OUTPUT};
    QString readFile(QUrl file_path);
    void saveFile(QUrl file_path, QString data);
    bool readyToSaveFile(QString data, SaveType type = TEXT);
    QVector<Tokens> callScanner();
    Parsers* callParsers(QVector<Tokens> token_stream);
    void changeLastLineColor(Qt::GlobalColor color);
    void sts(const QString &type, const QString &lexeme, double value, const QString &func_ptr);
    void stsOut(Tokens token);
    QString ptsD(QVector<double> values);
    QString ptsS(QVector<QString> values);
    QString ptsE(QVector<struct ExprNode*> values);
    QString ptsOut(Parsers p);
    void outputScannerResult(QVector<Tokens> token_stream);
    void outputParsersResult(Parsers p);
    void outputPainterResult();

private:
    QUrl file_path;
    bool jud_status_change = false;
    QTextEdit *te_result;

public slots:
    void parsersOutputReceive(const QString &text, Qt::GlobalColor color);
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "codeeditor.h"
#include "compiler.h"
#include "drawwidget.h"

#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>

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
    void on_a_new_file_triggered();

    void on_a_open_file_triggered();

    void on_a_save_flie_triggered();

    void on_a_font_triggered();

    void on_a_color_triggered();

    void on_a_compile_triggered();

    void on_a_run_triggered();

    void on_a_compile_and_run_triggered();

    void on_a_save_as_triggered();

    void on_a_output_JPG_triggered();

    void on_a_output_PNG_triggered();

    void on_a_output_BMP_triggered();

    void on_a_output_nodes_triggered();

private:
    Ui::MainWindow *ui;
    CodeEditor *ce;
    QTextEdit *te_result;
    DrawWidget *dw = NULL;
    QLabel *l_file_status;
    QCheckBox *cb_auto_output_nodes;

public:
    enum Status{COMPILED, UNCOMPILED};
    enum FileType {TEXT, CODE, NODE, NODE_THIS, AS};
    QString readFile(QUrl file_path);
    void saveFile(QUrl file_path, QString data);
    bool readyToReadFile(FileType type = TEXT);
    bool readyToSaveFile(QString data, FileType type = TEXT);
    bool readNodes(QString data);
    bool readTexts(QString data);
    void setCRAvail(bool jud);
    void setCRAvail(QString suffix);
    void setOutputAvail(bool jud);
    bool callCompiler();
    void callDraw();
    void outputPicture(QString format);
    void changeLastLineColor(Qt::GlobalColor color);

private:
    QUrl file_path;
    QList<QUrl> temp_path;
    bool jud_status_change = false;
    Status status;

public slots:
    void clearReceive();
    void resultReceive();
    void processReceive(QString process, Qt::GlobalColor color);
    void errorReceive(QString error, Compiler::Status compiler_status, Scanner::Status scanner_status, Parsers::Status parsers_status);
};
#endif // MAINWINDOW_H

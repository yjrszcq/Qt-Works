#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "codeeditor.h"
#include "drawdialog.h"
#include "globaldefines.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QTextEdit>


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

private:
    Ui::MainWindow *ui;
    CodeEditor *ce;
    DrawDialog *dd = NULL;

public:
    enum FileType {TEXT, CODE, NODE, NODE_THIS, AS};
    QString readFile(QUrl file_path);
    void saveFile(QUrl file_path, QString data);
    bool readyToReadFile(FileType type = TEXT);
    bool readyToSaveFile(QString data, FileType type = TEXT);
    bool readNodes(QString data);
    bool readTexts(QString data);
    void setCREnabled(bool jud);
    void setCREnabled(QString suffix);
    void callScanner();
    void callParsers();
    void callDraw();
    void changeLastLineColor(Qt::GlobalColor color);
    void sts(const QString &type, const QString &lexeme, double value, const QString &func_ptr);
    void stsOut(Tokens token);
    void outputScannerResult();
    void initParsers();
    void nodeTotalXY(QTextStream &out_total, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y);
    void nodeXY(QTextStream &out_total, double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b);
    void outTextXY(QTextStream &out_text, double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b);
    void outputParsersResult();
    void outputPainterResult();

private:
    QUrl file_path;
    bool jud_status_change = false;
    QTextEdit *te_result;

public slots:
    void parsersOutputReceive(const QString &text, Qt::GlobalColor color);
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "codeeditor.h"
#include "scanner.h"

#include <QFileDialog>
#include <QFontDialog>
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
    void on_new_file_triggered();
    void on_open_file_triggered();

    void on_save_flie_triggered();

    void on_font_triggered();

    void on_compile_triggered();

private:
    Ui::MainWindow *ui;
    CodeEditor *ce;

public:
    QString readFile(QUrl file_path);
    void saveFile(QUrl file_path);
    QVector<Tokens> callScanner();
    void ts(const QString &type, const QString &lexeme, double value, const QString &func_ptr);
    void tsOut(Tokens token);

private:
    QUrl file_path;
    bool jud_status_change = false;
    QTextEdit *te_result;
};
#endif // MAINWINDOW_H

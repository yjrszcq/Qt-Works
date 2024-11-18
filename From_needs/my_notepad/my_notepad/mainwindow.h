#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "codeeditor.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>

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

private:
    Ui::MainWindow *ui;
    CodeEditor *ce;

public:
    QString readFile(QUrl file_path);
    void saveFile(QUrl file_path);

private:
    QUrl file_path;
    bool jud_status_change = false;
};
#endif // MAINWINDOW_H

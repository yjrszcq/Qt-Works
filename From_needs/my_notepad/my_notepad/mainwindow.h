#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>

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
    void on_action_color_triggered();

    void on_action_font_triggered();

    void on_action_open_triggered();

    void on_textEdit_text_textChanged();

private:
    Ui::MainWindow *ui;

private:
    QUrl file_url;
    bool text_changed_flag;
};
#endif // MAINWINDOW_H

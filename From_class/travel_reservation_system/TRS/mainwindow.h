#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mysqldb.h"
#include "servor.h"

#include <QMainWindow>
#include <QDebug>
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

private:
    Ui::MainWindow *ui;

public:
    MysqlDb* db;
    Servor *servor;

private:
    bool connectToServor();

public slots:
    void exitReceive(int flag);
};
#endif // MAINWINDOW_H

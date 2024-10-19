#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mysqldb.h"
#include "servor.h"

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>

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
    MysqlDb* db = NULL;
    Servor *servor = NULL;

private:
    QStandardItemModel *model = NULL;
    bool connectToServor();
    void initializeModel(int flag);
    void opentable(QList<QHash<QString,QString>> data, int flag);
    bool loadData(int flag);
    void setUserAvailable(int flag);
public slots:
    void exitReceive(int flag);
private slots:
    void on_tv_display_clicked(const QModelIndex &index);
    void on_cb_option_currentIndexChanged(int index);
};
#endif // MAINWINDOW_H

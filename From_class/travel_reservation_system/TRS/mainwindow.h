#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mysqldb.h"
#include "server.h"

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
    Server *server = NULL;

private:
    QStandardItemModel *model = NULL;
    bool connectToServer();
    void initializeModel(int flag);
    void opentable(QList<QHash<QString,QString>> data, int flag);
    void loadData(int flag);
    void setUserAvailable(int flag);

public slots:
    void exitReceive(int flag);
    void userReceive(User* currentUser);
    void refreshReceive(int flag);

private slots:
    void on_tv_display_clicked(const QModelIndex &index);
    void on_cb_option_currentIndexChanged(int index);
    void on_pb_log_in_clicked();
    void on_pb_log_out_clicked();
    void on_pb_sign_up_clicked();
    void on_pb_reservation_clicked();
};
#endif // MAINWINDOW_H

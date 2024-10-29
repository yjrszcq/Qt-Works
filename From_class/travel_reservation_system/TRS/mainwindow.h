#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
    Server *server = NULL;
    QStandardItemModel *model = NULL;
    QHash<QString, int> flight_difference;
    QHash<QString, int> hotel_difference;
    QHash<QString, int> bus_difference;
    bool connectToServer();
    void refreshMainPage(int flag);
    void refreshMainPage(QList<QHash<QString,QString>> data, int flag);
    void setUserVisibility(User::Permission permission);
    void setUserAvailable(User::Permission permission);
    void functionVisibility(bool flag);
    void initializeModel(int flag);
    void initializeDataMap(int flag);
    void loadData(int flag);
    void loadData(QList<QHash<QString,QString>> data, int flag);
    void opentable(QList<QHash<QString,QString>> data, int flag);
    void dataMapper(QList<QString> rowData);
    void showSearchBar(bool flag);
    void refreshSearchComboBox(int flag);

    // my_resv
    QList<QHash<QString,QString>> data;
    Reservation *resv = NULL;
    QStandardItemModel *model_my_resv = NULL;
    int currentPage;
    void functionVisibilityMyResv(int flag);
    void initializeModelMyResv();
    void loadDataMyResv(bool unavail_display);
    void opentableMyResv(bool unavail_display);
    void refreshResvVisibilityMyResv(Qt::CheckState state);

public slots:
    void userReceive(User* currentUser);
    void refreshReceive(int flag);

private slots:
    void on_tv_display_clicked(const QModelIndex &index);
    void on_cb_option_currentIndexChanged(int index);
    void on_pb_log_in_clicked();
    void on_pb_log_out_clicked();
    void on_pb_sign_up_clicked();
    void on_pb_reservation_clicked();
    void on_sb_flight_1_valueChanged(int arg1);
    void on_sb_hotel_1_valueChanged(int arg1);
    void on_sb_bus_1_valueChanged(int arg1);
    void on_sb_flight_2_valueChanged(int arg1);
    void on_sb_hotel_2_valueChanged(int arg1);
    void on_sb_bus_2_valueChanged(int arg1);
    void on_pb_insert_clicked();
    void on_pb_update_clicked();
    void on_pb_delete_clicked();
    void on_pb_search_clicked();
    void on_pb_hide_search_clicked();
    void on_pb_search_ok_clicked();
    void on_pb_my_reservation_clicked();

    // my_resv
    void on_pb_back_main_clicked();
    void on_cb_display_unavail_my_resv_stateChanged(int arg1);
    void on_tv_display_my_resv_clicked(const QModelIndex &index);
    void on_pb_my_resv_content_clicked();
    void on_pb_alter_my_resv_note_clicked();
    void on_pb_alter_ok_clicked();
    void on_pb_cancel_my_resv_clicked();
    void on_pb_delete_my_resv_clicked();
    void on_pb_my_resv_track_clicked();
};
#endif // MAINWINDOW_H

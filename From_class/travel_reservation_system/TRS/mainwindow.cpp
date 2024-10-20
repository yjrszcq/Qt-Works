#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"
#include "signupdialog.h"

#include <QMessageBox>
#include <QListView>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(!connectToServer()){
        QMessageBox::critical(this , "错误", "Server连接失败", QMessageBox::Yes);
        QCoreApplication::exit(2);
    }
    ui->tv_display->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tv_display->setAlternatingRowColors(true);
    ui->cb_option->setCurrentIndex(1);
    ui->cb_option->setCurrentIndex(0);
    setUserAvailable(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}

void MainWindow::exitReceive(int flag){
    QCoreApplication::exit(flag);
}

void MainWindow::userReceive(User* currentUser){
    User::Permission permission = currentUser->getPermission();
    setUserAvailable(permission);
    ui->cb_option->setCurrentIndex(0);
    if(currentUser->getName() != ""){
        ui->l_user_display->setText(currentUser->getName());
    } else{
        ui->l_user_display->setText("未登录");
    }

}

bool MainWindow::connectToServer(){
    server = new Server();
    connect(server, SIGNAL(exitSent(int)), this, SLOT(exitReceive(int)));
    connect(server, SIGNAL(userSent(User*)), this, SLOT(userReceive(User*)));
    if(server != NULL){
        return true;
    } else{
        return false;
    }
}

void MainWindow::setUserAvailable(int userPermission){
    QListView* view = qobject_cast<QListView *>(ui->cb_option->view());
    Q_ASSERT(view != nullptr);
    switch(userPermission){
    case User::VISITOR: {
        view->setRowHidden(3, true);
        view->setRowHidden(4, true);
        ui->pb_insert->hide();
        ui->pb_update->hide();
        ui->pb_delete->hide();
        ui->pb_my_reservation->hide();
        ui->pb_reservation->hide();
        ui->pb_log_in->show();
        ui->pb_sign_up->show();
        ui->pb_log_out->hide();
        ui->le_detail_0->setReadOnly(true);
        ui->le_detail_1->setReadOnly(true);
        ui->le_detail_2->setReadOnly(true);
        ui->le_detail_3->setReadOnly(true);
        ui->le_detail_4->setReadOnly(true);
        ui->le_detail_5->setReadOnly(true);
        break;
    }
    case User::USER: {
        view->setRowHidden(3, true);
        view->setRowHidden(4, true);
        ui->pb_insert->hide();
        ui->pb_update->hide();
        ui->pb_delete->hide();
        ui->pb_my_reservation->show();
        ui->pb_reservation->show();
        ui->pb_log_in->hide();
        ui->pb_sign_up->hide();
        ui->pb_log_out->show();
        ui->le_detail_0->setReadOnly(true);
        ui->le_detail_1->setReadOnly(true);
        ui->le_detail_2->setReadOnly(true);
        ui->le_detail_3->setReadOnly(true);
        ui->le_detail_4->setReadOnly(true);
        ui->le_detail_5->setReadOnly(true);
        break;
    }
    case User::ROOT: {
        view->setRowHidden(3, false);
        view->setRowHidden(4, false);
        ui->pb_insert->show();
        ui->pb_update->show();
        ui->pb_delete->show();
        ui->pb_my_reservation->hide();
        ui->pb_reservation->hide();
        ui->pb_log_in->hide();
        ui->pb_sign_up->hide();
        ui->pb_log_out->show();
        ui->le_detail_0->setReadOnly(false);
        ui->le_detail_1->setReadOnly(false);
        ui->le_detail_2->setReadOnly(false);
        ui->le_detail_3->setReadOnly(false);
        ui->le_detail_4->setReadOnly(false);
        ui->le_detail_5->setReadOnly(false);
        break;
    }
    }
}

void MainWindow::initializeModel(int flag){
    if(model != NULL){
        delete model;
    }
    model = new QStandardItemModel();
    switch(flag){
    case 0: model->setHorizontalHeaderLabels({"航班号", "价格", "座位总数", "剩余座位", "出发地", "到达地"}); break;
    case 1: model->setHorizontalHeaderLabels({"所在地", "价格", "房间总数", "剩余房间"}); break;
    case 2: model->setHorizontalHeaderLabels({"所在地", "价格", "客车总数", "剩余可选"}); break;
    case 3: model->setHorizontalHeaderLabels({"客户", "ID"}); break;
    case 4: model->setHorizontalHeaderLabels({"客户", "预订类型", "预订ID"}); break;
    }
}

void MainWindow::opentable(QList<QHash<QString,QString>> data, int flag){
    initializeModel(flag);
    switch(flag){
    case 0:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(data[i]["flightNum"]));
            row.append(new QStandardItem(data[i]["price"]));
            row.append(new QStandardItem(data[i]["numSeats"]));
            row.append(new QStandardItem(data[i]["numAvail"]));
            row.append(new QStandardItem(data[i]["FromCity"]));
            row.append(new QStandardItem(data[i]["ArivCity"]));
            model->appendRow(row);
        }
        break;
    }
    case 1:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(data[i]["location"]));
            row.append(new QStandardItem(data[i]["price"]));
            row.append(new QStandardItem(data[i]["numRooms"]));
            row.append(new QStandardItem(data[i]["numAvail"]));
            model->appendRow(row);
        }
        break;
    }
    case 2:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(data[i]["location"]));
            row.append(new QStandardItem(data[i]["price"]));
            row.append(new QStandardItem(data[i]["numBus"]));
            row.append(new QStandardItem(data[i]["numAvail"]));
            model->appendRow(row);
        }
        break;
    }
    case 3:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(data[i]["custName"]));
            row.append(new QStandardItem(data[i]["custID"]));
            model->appendRow(row);
        }
        break;
    }
    case 4:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(data[i]["custName"]));
            row.append(new QStandardItem(data[i]["resvType"]));
            row.append(new QStandardItem(data[i]["resvKey"]));
            model->appendRow(row);
        }
        break;
    }
    }
    ui->tv_display->setModel(model);
}

bool MainWindow::loadData(int flag){
    QList<QHash<QString,QString>> data = server->getData(flag);
    qDebug("get data succeed");
    opentable(data, flag);
}

void MainWindow::on_tv_display_clicked(const QModelIndex &index)
{
    QList<QString> rowData;
    int row = index.row();
    int columnCount = index.model()->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        QModelIndex cellIndex = index.model()->index(row, col);
        rowData.append(index.model()->data(cellIndex).toString());
    }
    ui->le_detail_0->setText(rowData[0]);
    ui->le_detail_1->setText(rowData[1]);
    if(columnCount >= 3){
        ui->le_detail_2->setText(rowData[2]);
        if(columnCount >= 4){
            ui->le_detail_3->setText(rowData[3]);
            if(columnCount > 4){
                ui->le_detail_4->setText(rowData[4]);
                ui->le_detail_5->setText(rowData[5]);
            }
        }
    }

}

void MainWindow::on_cb_option_currentIndexChanged(int index)
{
    switch(index){
    case 0:{
        ui->l_detail_0->setText("航班号"); ui->l_detail_0->show();
        ui->l_detail_1->setText("价格"); ui->l_detail_1->show();
        ui->l_detail_2->setText("座位总数"); ui->l_detail_2->show();
        ui->l_detail_3->setText("剩余座位"); ui->l_detail_3->show();
        ui->l_detail_4->setText("出发地"); ui->l_detail_4->show();
        ui->l_detail_5->setText("到达地"); ui->l_detail_5->show();

        ui->le_detail_0->clear(); ui->le_detail_0->show();
        ui->le_detail_1->clear(); ui->le_detail_1->show();
        ui->le_detail_2->clear(); ui->le_detail_2->show();
        ui->le_detail_3->clear(); ui->le_detail_3->show();
        ui->le_detail_4->clear(); ui->le_detail_4->show();
        ui->le_detail_5->clear(); ui->le_detail_5->show();
        break;
    }
    case 1:{
        ui->l_detail_0->setText("所在地"); ui->l_detail_0->show();
        ui->l_detail_1->setText("价格"); ui->l_detail_1->show();
        ui->l_detail_2->setText("房间总数"); ui->l_detail_2->show();
        ui->l_detail_3->setText("剩余房间"); ui->l_detail_3->show();
        ui->l_detail_4->hide();
        ui->l_detail_5->hide();

        ui->le_detail_0->clear(); ui->le_detail_0->show();
        ui->le_detail_1->clear(); ui->le_detail_1->show();
        ui->le_detail_2->clear(); ui->le_detail_2->show();
        ui->le_detail_3->clear(); ui->le_detail_3->show();
        ui->le_detail_4->clear(); ui->le_detail_4->hide();
        ui->le_detail_5->clear(); ui->le_detail_5->hide();
        break;
    }
    case 2:{
        ui->l_detail_0->setText("所在地"); ui->l_detail_0->show();
        ui->l_detail_1->setText("价格"); ui->l_detail_1->show();
        ui->l_detail_2->setText("客车总数"); ui->l_detail_2->show();
        ui->l_detail_3->setText("剩余可选"); ui->l_detail_3->show();
        ui->l_detail_4->hide();
        ui->l_detail_5->hide();

        ui->le_detail_0->clear(); ui->le_detail_0->show();
        ui->le_detail_1->clear(); ui->le_detail_1->show();
        ui->le_detail_2->clear(); ui->le_detail_2->show();
        ui->le_detail_3->clear(); ui->le_detail_3->show();
        ui->le_detail_4->clear(); ui->le_detail_4->hide();
        ui->le_detail_5->clear(); ui->le_detail_5->hide();
        break;
    }
    case 3:{
        ui->l_detail_0->setText("客户"); ui->l_detail_0->show();
        ui->l_detail_1->setText("ID"); ui->l_detail_1->show();
        ui->l_detail_2->hide();
        ui->l_detail_3->hide();
        ui->l_detail_4->hide();
        ui->l_detail_5->hide();

        ui->le_detail_0->clear(); ui->le_detail_0->show();
        ui->le_detail_1->clear(); ui->le_detail_1->show();
        ui->le_detail_2->clear(); ui->le_detail_2->hide();
        ui->le_detail_3->clear(); ui->le_detail_3->hide();
        ui->le_detail_4->clear(); ui->le_detail_4->hide();
        ui->le_detail_5->clear(); ui->le_detail_5->hide();
        break;
    }
    case 4:{
        ui->l_detail_0->setText("客户"); ui->l_detail_0->show();
        ui->l_detail_1->setText("预订类型"); ui->l_detail_1->show();
        ui->l_detail_2->setText("预订ID"); ui->l_detail_2->show();
        ui->l_detail_3->hide();
        ui->l_detail_4->hide();
        ui->l_detail_5->hide();

        ui->le_detail_0->clear(); ui->le_detail_0->show();
        ui->le_detail_1->clear(); ui->le_detail_1->show();
        ui->le_detail_2->clear(); ui->le_detail_2->show();
        ui->le_detail_3->clear(); ui->le_detail_3->hide();
        ui->le_detail_4->clear(); ui->le_detail_4->hide();
        ui->le_detail_5->clear(); ui->le_detail_5->hide();
        break;
    }
    }

    loadData(index);

}


void MainWindow::on_pb_log_in_clicked()
{
    LoginDialog::show(server);
}


void MainWindow::on_pb_log_out_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::warning(this, "提示", "是否退出登录？", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes){
        server->logout();
    }
}


void MainWindow::on_pb_sign_up_clicked()
{
    SignupDialog::show(server);
}


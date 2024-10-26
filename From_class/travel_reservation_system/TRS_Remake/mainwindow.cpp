#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"
#include "signupdialog.h"
#include "adddialog.h"
#include "myresvdialog.h"

#include <QMessageBox>
#include <QListView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("旅行预订系统");
    if(!connectToServer()){
        QMessageBox::critical(this , "错误", "Server连接失败，程序关闭", QMessageBox::Yes);
        QCoreApplication::exit(2);
    }
    ui->tv_display->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tv_display->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tv_display->setAlternatingRowColors(true);

    ui->tv_display_my_resv->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tv_display_my_resv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tv_display_my_resv->setAlternatingRowColors(true);

    ui->sw_main->setCurrentIndex(0);
    ui->cb_option->setCurrentIndex(1);
    ui->cb_option->setCurrentIndex(0);
    setUserVisibility(User::VISITOR);
    setUserAvailable(User::VISITOR);

    showSearchBar(false);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
    delete model;
    delete model_my_resv;
    delete[] resv;
}

void MainWindow::userReceive(User* currentUser){
    User::Permission permission = currentUser->getPermission();
    setUserVisibility(permission);
    setUserAvailable(permission);
    ui->cb_option->setCurrentIndex(1);
    ui->cb_option->setCurrentIndex(0);
    if(currentUser->getId() != ""){
        ui->l_user_display->setText(currentUser->getId());
    } else{
        ui->l_user_display->setText(currentUser->getName());
    }

}

void MainWindow::refreshReceive(int flag){
    refreshMainPage(flag);
}

bool MainWindow::connectToServer(){
    server = new Server();
    connect(server, SIGNAL(userSent(User*)), this, SLOT(userReceive(User*)));
    connect(server, SIGNAL(refreshSent(int)), this, SLOT(refreshReceive(int)));
    if(server != NULL && server->getStatus() == Server::AVAILABLE){
        return true;
    } else{
        return false;
    }
}

void MainWindow::refreshMainPage(int flag){
    loadData(flag);
    initializeDataMap(flag);
    rootFunctionVisibility(false);
    refreshSearchComboBox(flag);
}

void MainWindow::refreshMainPage(QList<QHash<QString,QString>> data, int flag){
    loadData(data, flag);
    initializeDataMap(flag);
    rootFunctionVisibility(false);
}

void MainWindow::setUserVisibility(User::Permission permission){
    QListView* view = qobject_cast<QListView *>(ui->cb_option->view());
    Q_ASSERT(view != nullptr);
    switch(permission){
    case User::VISITOR: {
        view->setRowHidden(3, true);
        view->setRowHidden(4, true);
        ui->sw_function->setCurrentIndex(0);
        ui->pb_my_reservation->hide();
        ui->pb_log_in->show();
        ui->pb_sign_up->show();
        ui->pb_log_out->hide();
        break;
    }
    case User::USER: {
        view->setRowHidden(3, true);
        view->setRowHidden(4, true);
        ui->sw_function->setCurrentIndex(1);
        ui->pb_my_reservation->show();
        ui->pb_log_in->hide();
        ui->pb_sign_up->hide();
        ui->pb_log_out->show();
        break;
    }
    case User::ROOT: {
        view->setRowHidden(3, false);
        view->setRowHidden(4, false);
        ui->sw_function->setCurrentIndex(2);
        ui->pb_my_reservation->hide();
        ui->pb_reservation->hide();
        ui->pb_log_in->hide();
        ui->pb_sign_up->hide();
        ui->pb_log_out->show();
        break;
    }
    }
}

void MainWindow::setUserAvailable(User::Permission permission){
    switch(permission){
    case User::VISITOR: case User::USER:{
        ui->sb_flight_0->setReadOnly(true);
        ui->sb_flight_1->setReadOnly(true);
        ui->sb_flight_2->setReadOnly(true);
        ui->le_flight_1->setReadOnly(true);
        ui->le_flight_2->setReadOnly(true);

        ui->sb_hotel_0->setReadOnly(true);
        ui->sb_hotel_1->setReadOnly(true);
        ui->sb_hotel_2->setReadOnly(true);

        ui->sb_bus_0->setReadOnly(true);
        ui->sb_bus_1->setReadOnly(true);
        ui->sb_bus_2->setReadOnly(true);
        break;
    }
    case User::ROOT:{
        ui->sb_flight_0->setReadOnly(false);
        ui->sb_flight_1->setReadOnly(false);
        ui->sb_flight_2->setReadOnly(false);
        ui->le_flight_1->setReadOnly(false);
        ui->le_flight_2->setReadOnly(false);

        ui->sb_hotel_0->setReadOnly(false);
        ui->sb_hotel_1->setReadOnly(false);
        ui->sb_hotel_2->setReadOnly(false);

        ui->sb_bus_0->setReadOnly(false);
        ui->sb_bus_1->setReadOnly(false);
        ui->sb_bus_2->setReadOnly(false);
        break;
    }
    }
}

void MainWindow::rootFunctionVisibility(bool flag){
    if(server->getCurrentUser()->getPermission() == User::ROOT){
        if(flag == false){
            ui->pb_update->hide();
            ui->pb_delete->hide();
        } else{
            ui->pb_update->show();
            ui->pb_delete->show();
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
    case 1: model->setHorizontalHeaderLabels({"宾馆地址", "价格", "房间总数", "剩余房间"}); break;
    case 2: model->setHorizontalHeaderLabels({"车站地址", "价格", "座位总数", "剩余座位"}); break;
    case 3: model->setHorizontalHeaderLabels({"客户", "ID", "密码"}); break;
    case 4: model->setHorizontalHeaderLabels({"客户", "预订类型", "预订内容", "预订ID", "预订状态", "预订备注"}); break;
    }
}

void MainWindow::initializeDataMap(int flag){
    switch(flag){
    case 0:{
        ui->le_flight_0->clear();
        ui->sb_flight_0->clear();
        ui->sb_flight_1->clear();
        ui->sb_flight_2->clear();
        ui->le_flight_1->clear();
        ui->le_flight_2->clear();
        break;
    }
    case 1:{
        ui->le_hotel_0->clear();
        ui->sb_hotel_0->clear();
        ui->sb_hotel_1->clear();
        ui->sb_hotel_2->clear();
        break;
    }
    case 2:{
        ui->le_bus_0->clear();
        ui->sb_bus_0->clear();
        ui->sb_bus_1->clear();
        ui->sb_bus_2->clear();
        break;
    }
    case 3:{
        ui->le_user_0->clear();
        ui->le_user_1->clear();
        ui->le_user_2->clear();
        break;
    }
    case 4:{
        ui->le_resv_0->clear();
        ui->le_resv_1->clear();
        ui->le_resv_2->clear();
        ui->le_resv_3->clear();
        ui->cb_resv_0->setCurrentIndex(0);
        ui->le_resv_4->clear();
        break;
    }
    }
}

void MainWindow::loadData(int flag){
    QList<QHash<QString,QString>> data = server->getData(flag);
    opentable(data, flag);
}

void MainWindow::loadData(QList<QHash<QString,QString>> data, int flag){
    opentable(data, flag);
}

void MainWindow::opentable(QList<QHash<QString,QString>> data, int flag){
    User::Permission permission = server->getCurrentUser()->getPermission();
    initializeModel(flag);
    flight_difference.clear();
    hotel_difference.clear();
    bus_difference.clear();
    switch(flag){
    case 0:{
        bool ok;
        for (int i = 0; i < data.size(); ++i) {
            flight_difference.insert(data[i]["flightNum"], (data[i]["numSeats"].toInt(&ok, 10) - data[i]["numAvail"].toInt(&ok, 10)));
            if(permission == User::ROOT || (permission != User::ROOT && data[i]["numAvail"] != "0")){
                QList<QStandardItem*> row;
                row.append(new QStandardItem(data[i]["flightNum"]));
                row.append(new QStandardItem(data[i]["price"]));
                row.append(new QStandardItem(data[i]["numSeats"]));
                row.append(new QStandardItem(data[i]["numAvail"]));
                row.append(new QStandardItem(data[i]["FromCity"]));
                row.append(new QStandardItem(data[i]["ArivCity"]));
                model->appendRow(row);
            }
        }
        break;
    }
    case 1:{
        bool ok;
        for (int i = 0; i < data.size(); ++i) {
            hotel_difference.insert(data[i]["location"], (data[i]["numRooms"].toInt(&ok, 10) - data[i]["numAvail"].toInt(&ok, 10)));
            if(permission == User::ROOT || (permission != User::ROOT && data[i]["numAvail"] != "0")){
                QList<QStandardItem*> row;
                row.append(new QStandardItem(data[i]["location"]));
                row.append(new QStandardItem(data[i]["price"]));
                row.append(new QStandardItem(data[i]["numRooms"]));
                row.append(new QStandardItem(data[i]["numAvail"]));
                model->appendRow(row);
            }
        }
        break;
    }
    case 2:{
        bool ok;
        for (int i = 0; i < data.size(); ++i) {
            bus_difference.insert(data[i]["location"], (data[i]["numBus"].toInt(&ok, 10) - data[i]["numAvail"].toInt(&ok, 10)));
            if(permission == User::ROOT || (permission != User::ROOT && data[i]["numAvail"] != "0")){
                QList<QStandardItem*> row;
                row.append(new QStandardItem(data[i]["location"]));
                row.append(new QStandardItem(data[i]["price"]));
                row.append(new QStandardItem(data[i]["numBus"]));
                row.append(new QStandardItem(data[i]["numAvail"]));
                model->appendRow(row);
            }
        }
        break;
    }
    case 3:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            row.append(new QStandardItem(data[i]["custName"]));
            row.append(new QStandardItem(data[i]["custID"]));
            row.append(new QStandardItem(data[i]["custPW"]));
            model->appendRow(row);
        }
        break;
    }
    case 4:{
        for (int i = 0; i < data.size(); ++i) {
            QList<QStandardItem*> row;
            bool ok;
            row.append(new QStandardItem(data[i]["custName"]));
            switch(data[i]["resvType"].toInt(&ok, 10)){
            case Reservation::FLIGHT: row.append(new QStandardItem("航班")); break;
            case Reservation::HOTEL: row.append(new QStandardItem("宾馆")); break;
            case Reservation::BUS: row.append(new QStandardItem("客车")); break;
            }
            row.append(new QStandardItem(data[i]["resvContent"]));
            row.append(new QStandardItem(data[i]["resvKey"]));
            row.append(new QStandardItem(data[i]["resvAvail"]));
            row.append(new QStandardItem(data[i]["resvNote"]));
            model->appendRow(row);
        }
        break;
    }
    }
    ui->tv_display->setModel(model);
}

void MainWindow::dataMapper(QList<QString> rowData){
    switch(ui->cb_option->currentIndex()){
    case 0:{
        bool ok;
        ui->le_flight_0->setText(rowData[0]);
        ui->sb_flight_0->setValue(rowData[1].toInt(&ok, 10));
        ui->sb_flight_1->setValue(rowData[2].toInt(&ok, 10));
        ui->sb_flight_2->setValue(rowData[3].toInt(&ok, 10));
        ui->le_flight_1->setText(rowData[4]);
        ui->le_flight_2->setText(rowData[5]);
        break;
    }
    case 1:{
        bool ok;
        ui->le_hotel_0->setText(rowData[0]);
        ui->sb_hotel_0->setValue(rowData[1].toInt(&ok, 10));
        ui->sb_hotel_1->setValue(rowData[2].toInt(&ok, 10));
        ui->sb_hotel_2->setValue(rowData[3].toInt(&ok, 10));
        break;
    }
    case 2:{
        bool ok;
        ui->le_bus_0->setText(rowData[0]);
        ui->sb_bus_0->setValue(rowData[1].toInt(&ok, 10));
        ui->sb_bus_1->setValue(rowData[2].toInt(&ok, 10));
        ui->sb_bus_2->setValue(rowData[3].toInt(&ok, 10));
        break;
    }
    case 3:{
        ui->le_user_0->setText(rowData[0]);
        ui->le_user_1->setText(rowData[1]);
        ui->le_user_2->setText(rowData[2]);
        break;
    }
    case 4:{
        bool ok;
        ui->le_resv_0->setText(rowData[0]);
        ui->le_resv_1->setText(rowData[1]);
        ui->le_resv_2->setText(rowData[2]);
        ui->le_resv_3->setText(rowData[3]);
        ui->cb_resv_0->setCurrentIndex(rowData[4].toInt(&ok, 10));
        ui->le_resv_4->setText(rowData[5]);
        break;
    }
    }
}

void MainWindow::showSearchBar(bool flag){
    if(flag){
        ui->pb_search->hide();
        ui->l_search->show();
        ui->le_search->show();
        ui->cb_search->show();
        ui->pb_hide_search->show();
        ui->pb_search_ok->show();
    } else{
        ui->pb_search->show();
        ui->l_search->hide();
        ui->le_search->hide();
        ui->cb_search->hide();
        ui->pb_hide_search->hide();
        ui->pb_search_ok->hide();
    }
}

void MainWindow::refreshSearchComboBox(int flag){
    ui->cb_search->clear();
    switch(flag){
    case 0:{
        ui->cb_search->addItem("航班号");
        ui->cb_search->addItem("出发地");
        ui->cb_search->addItem("到达地");
        break;
    }
    case 1:{
        ui->cb_search->addItem("宾馆地址");
        break;
    }
    case 2:{
        ui->cb_search->addItem("车站地址");
        break;
    }
    case 3:{
        ui->cb_search->addItem("用户名");
        break;
    }
    case 4:{
        ui->cb_search->addItem("预订ID");
        ui->cb_search->addItem("用户名");
        ui->cb_search->addItem("航班号");
        ui->cb_search->addItem("宾馆地址");
        ui->cb_search->addItem("车站地址");
        break;
    }
    }
}


void MainWindow::on_tv_display_clicked(const QModelIndex &index)
{
    rootFunctionVisibility(true);
    QList<QString> rowData;
    int row = index.row();
    int columnCount = index.model()->columnCount();
    for (int col = 0; col < columnCount; ++col) {
        QModelIndex cellIndex = index.model()->index(row, col);
        rowData.append(index.model()->data(cellIndex).toString());
    }
    dataMapper(rowData);
}


void MainWindow::on_cb_option_currentIndexChanged(int index)
{
    ui->sw_detail->setCurrentIndex(index);
    refreshMainPage(index);
}


void MainWindow::on_pb_log_in_clicked()
{
    LoginDialog::show(server);
}


void MainWindow::on_pb_log_out_clicked()
{
    server->logout();
}


void MainWindow::on_pb_sign_up_clicked()
{
    SignupDialog::show(server);
}


void MainWindow::on_pb_reservation_clicked()
{
    QString text = "";
    int index = ui->cb_option->currentIndex();
    switch(index){
    case 0: text = ui->le_flight_0->text(); break;
    case 1: text = ui->le_hotel_0->text(); break;
    case 2: text = ui->le_bus_0->text(); break;
    }
    server->reserve(Reservation(server->getCurrentUser()->getName(), index, text));
}


void MainWindow::on_sb_flight_1_valueChanged(int arg1)
{
    int difference = flight_difference[ui->le_flight_0->text()];
    if(arg1 - difference < 0){
        ui->sb_flight_1->setValue(ui->sb_flight_2->value() + difference);
    } else{
        ui->sb_flight_2->setValue(arg1 - difference);
    }

}


void MainWindow::on_sb_hotel_1_valueChanged(int arg1)
{
    int difference = hotel_difference[ui->le_hotel_0->text()];
    if(arg1 - difference < 0){
        ui->sb_hotel_1->setValue(ui->sb_hotel_2->value() + difference);
    } else{
        ui->sb_hotel_2->setValue(arg1 - difference);
    }
}


void MainWindow::on_sb_bus_1_valueChanged(int arg1)
{
    int difference = bus_difference[ui->le_bus_0->text()];
    if(arg1 - difference < 0){
        ui->sb_bus_1->setValue(ui->sb_bus_2->value() + difference);
    } else{
        ui->sb_bus_2->setValue(arg1 - difference);
    }
}


void MainWindow::on_sb_flight_2_valueChanged(int arg1)
{
    ui->sb_flight_1->setValue(arg1 + flight_difference[ui->le_flight_0->text()]);
}


void MainWindow::on_sb_hotel_2_valueChanged(int arg1)
{
    ui->sb_hotel_1->setValue(arg1 + hotel_difference[ui->le_hotel_0->text()]);
}


void MainWindow::on_sb_bus_2_valueChanged(int arg1)
{
    ui->sb_bus_1->setValue(arg1 + bus_difference[ui->le_bus_0->text()]);
}


void MainWindow::on_pb_insert_clicked()
{
    AddDialog::show(server, ui->cb_option->currentIndex());
}


void MainWindow::on_pb_update_clicked()
{
    switch(ui->cb_option->currentIndex()){
    case 0: server->updateItem(Flight(
            ui->le_flight_0->text(),
            ui->sb_flight_0->value(),
            ui->sb_flight_1->value(),
            ui->sb_flight_2->value(),
            ui->le_flight_1->text(),
            ui->le_flight_2->text())); break;
    case 1: server->updateItem(Hotel(
            ui->le_hotel_0->text(),
            ui->sb_hotel_0->value(),
            ui->sb_hotel_1->value(),
            ui->sb_hotel_2->value())); break;
    case 2: server->updateItem(Bus(
            ui->le_bus_0->text(),
            ui->sb_bus_0->value(),
            ui->sb_bus_1->value(),
            ui->sb_bus_2->value())); break;
    case 3: server->updateItem(User(
            ui->le_user_0->text(),
            ui->le_user_1->text(),
            ui->le_user_2->text(),
            User::USER)); break;
    case 4: {
        int type;
        if(ui->le_resv_1->text() == "航班"){
            type = 0;
        } else if(ui->le_resv_1->text() == "宾馆"){
            type = 1;
        } else if(ui->le_resv_1->text() == "客车"){
            type = 2;
        } else{
            QMessageBox::critical(this, "错误", "修改失败-未知类型");
            return;
        }
        server->updateItem(Reservation(ui->le_resv_0->text(), type,
                                       ui->le_resv_2->text(), ui->le_resv_3->text(),
                                       ui->cb_resv_0->currentIndex(), ui->le_resv_4->text())); break;
    }
    }
}


void MainWindow::on_pb_delete_clicked()
{
    QString content = "";
    int index = ui->cb_option->currentIndex();
    switch(index){
    case 0: content = ui->le_flight_0->text(); break;
    case 1: content = ui->le_hotel_0->text(); break;
    case 2: content = ui->le_bus_0->text(); break;
    case 3: content = ui->le_user_0->text(); break;
    case 4: content = ui->le_resv_3->text(); break;
    }
    server->deleteItem(index, content);
}


void MainWindow::on_pb_search_clicked()
{
    ui->le_search->clear();
    ui->cb_search->setCurrentIndex(0);
    showSearchBar(true);
}


void MainWindow::on_pb_hide_search_clicked()
{
    showSearchBar(false);
    ui->le_search->clear();
    refreshMainPage(ui->cb_option->currentIndex());
}


void MainWindow::on_pb_search_ok_clicked()
{
    QList<QHash<QString,QString>> data = server->getData(
        ui->cb_option->currentIndex(),
        ui->cb_search->currentIndex(),
        ui->le_search->text());
    refreshMainPage(data, ui->cb_option->currentIndex());
}


void MainWindow::on_pb_my_reservation_clicked()
{
    ui->sw_main->setCurrentIndex(1);
    currentPage = ui->cb_option->currentIndex();
    User *currentUser = server->getCurrentUser();
    if(currentUser->getId() != ""){
        ui->l_user_display_my_resv->setText(currentUser->getId());
    } else{
        ui->l_user_display_my_resv->setText(currentUser->getName());
    }
    functionVisibilityMyResv(0);
    loadDataMyResv(0);
}


// my_resv
void MainWindow::functionVisibilityMyResv(int flag){
    ui->l_alter_note->hide();
    ui->le_alter_note->hide();
    ui->pb_alter_ok->hide();
    switch(flag){
    case 0: {
        ui->pb_my_resv_content->hide();
        ui->pb_alter_my_resv_note->hide();
        ui->pb_cancel_my_resv->hide();
        ui->pb_delete_my_resv->hide();
        break;
    }
    case 1: {
        ui->pb_my_resv_content->show();
        ui->pb_my_resv_content->setEnabled(true);
        ui->pb_alter_my_resv_note->show();
        ui->pb_cancel_my_resv->show();
        ui->pb_delete_my_resv->hide();
        break;
    }
    case 2: {
        ui->pb_my_resv_content->show();
        ui->pb_my_resv_content->setEnabled(false);
        ui->pb_alter_my_resv_note->show();
        ui->pb_cancel_my_resv->hide();
        ui->pb_delete_my_resv->show();
        break;
    }
    }
}

void MainWindow::loadDataMyResv(bool unavail_display){
    data.clear();
    data = server->getData(*(server->getCurrentUser()));
    opentableMyResv(unavail_display);
}

void MainWindow::initializeModelMyResv(){
    if(model_my_resv != NULL){
        delete model_my_resv;
    }
    model_my_resv = new QStandardItemModel();
    model_my_resv->setHorizontalHeaderLabels({"用户名", "预订类型", "预订内容", "预订ID", "预订状态", "预订备注"});
}

void MainWindow::opentableMyResv(bool unavail_display){
    initializeModelMyResv();
    for (int i = 0; i < data.size(); ++i) {
        if(unavail_display || data[i]["resvAvail"] == QString::number(Reservation::AVAILABLE)){
            QList<QStandardItem*> row;
            bool ok;
            row.append(new QStandardItem(data[i]["custName"]));
            switch(data[i]["resvType"].toInt(&ok, 10)){
            case Reservation::FLIGHT: row.append(new QStandardItem("航班")); break;
            case Reservation::HOTEL: row.append(new QStandardItem("宾馆")); break;
            case Reservation::BUS: row.append(new QStandardItem("客车")); break;
            }
            row.append(new QStandardItem(data[i]["resvContent"]));
            row.append(new QStandardItem(data[i]["resvKey"]));
            row.append(new QStandardItem(data[i]["resvAvail"]));
            row.append(new QStandardItem(data[i]["resvNote"]));
            model_my_resv->appendRow(row);
        }
    }
    ui->tv_display_my_resv->setModel(model_my_resv);
}

void MainWindow::refreshResvVisibilityMyResv(Qt::CheckState state){
    if(state == Qt::Checked){
        ui->cb_display_unavail_my_resv->setCheckState(Qt::Unchecked);
        ui->cb_display_unavail_my_resv->setCheckState(Qt::Checked);
    } else{
        ui->cb_display_unavail_my_resv->setCheckState(Qt::Checked);
        ui->cb_display_unavail_my_resv->setCheckState(Qt::Unchecked);
    }
}


void MainWindow::on_pb_back_main_clicked()
{
    ui->sw_main->setCurrentIndex(0);
    loadData(currentPage);
    initializeDataMap(currentPage);
}


void MainWindow::on_cb_display_unavail_my_resv_stateChanged(int arg1)
{
    functionVisibilityMyResv(0);
    opentableMyResv(arg1);
}


void MainWindow::on_tv_display_my_resv_clicked(const QModelIndex &index)
{
    QList<QString> rowData;
    int row = index.row();
    int columnCount = index.model()->columnCount();
    for (int col = 0; col < columnCount; ++col) {
        QModelIndex cellIndex = index.model()->index(row, col);
        rowData.append(index.model()->data(cellIndex).toString());
    }
    int type;
    if(rowData[1] == "航班"){
        type = 0;
    } else if(rowData[1] == "宾馆"){
        type = 1;
    } else if(rowData[1] == "客车"){
        type = 2;
    } else{
        QMessageBox::critical(this, "错误", "未知类型");
        return;
    }
    if(this->resv != NULL){
        delete resv;
    }
    bool ok;
    this->resv = new Reservation(rowData[0], type, rowData[2], rowData[3], rowData[4].toInt(&ok, 10), rowData[5]);
    ui->le_alter_note->setText(rowData[5]);
    if(rowData[4] == "1"){
        functionVisibilityMyResv(1);
    } else{
        functionVisibilityMyResv(2);
    }
}


void MainWindow::on_pb_my_resv_content_clicked()
{
    QHash<QString, QString> data = server->getData(resv->getResvType(), resv->getResvContent());
    if(data.size() == 0){
        QMessageBox::critical(this, "错误", "该内容不存在", QMessageBox::Yes);
        return;
    }
    bool ok;
    switch(resv->getResvType()){
    case 0: MyResvDialog::show(Flight(
            data["flightNum"],
            data["price"].toInt(&ok, 10),
            data["numSeats"].toInt(&ok, 10),
            data["numAvail"].toInt(&ok, 10),
            data["FromCity"], data["ArivCity"])); break;
    case 1: MyResvDialog::show(Hotel(
            data["location"],
            data["price"].toInt(&ok, 10),
            data["numRooms"].toInt(&ok, 10),
            data["numAvail"].toInt(&ok, 10))); break;
    case 2: MyResvDialog::show(Bus(
            data["location"],
            data["price"].toInt(&ok, 10),
            data["numBus"].toInt(&ok, 10),
            data["numAvail"].toInt(&ok, 10))); break;
    }
}


void MainWindow::on_pb_alter_my_resv_note_clicked()
{
    ui->l_alter_note->show();
    ui->le_alter_note->show();
    ui->pb_alter_ok->show();
    ui->pb_alter_my_resv_note->hide();
}


void MainWindow::on_pb_alter_ok_clicked()
{
    resv->setResvNote(ui->le_alter_note->text());
    server->updateItem(*resv);
    functionVisibilityMyResv(0);
    loadDataMyResv(0);
    refreshResvVisibilityMyResv(ui->cb_display_unavail_my_resv->checkState());
}


void MainWindow::on_pb_cancel_my_resv_clicked()
{
    resv->setResvAvail(0);
    resv->setResvNote("用户取消预订");
    server->updateItem(*resv);
    functionVisibilityMyResv(0);
    loadDataMyResv(0);
    refreshResvVisibilityMyResv(ui->cb_display_unavail_my_resv->checkState());
}


void MainWindow::on_pb_delete_my_resv_clicked()
{
    server->deleteItem(4, resv->getResvKey());
    functionVisibilityMyResv(0);
    loadDataMyResv(0);
    refreshResvVisibilityMyResv(ui->cb_display_unavail_my_resv->checkState());
}


void MainWindow::on_pb_my_resv_track_clicked()
{

}

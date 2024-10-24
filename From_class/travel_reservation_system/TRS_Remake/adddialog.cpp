#include "adddialog.h"
#include "ui_adddialog.h"

AddDialog::AddDialog(Server *server, int flag, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("添加项目");
    this->server = server;
    this->flag = flag;
    ui->sw_detail->setCurrentIndex(flag);
    if(flag == 4){
        QList<QHash<QString, QString>> users = server->getData(3);
        for(auto user : users){
            ui->cb_resv_0->addItem(user["custName"]);
        }
        ui->cb_resv_1->setCurrentIndex(1);
        ui->cb_resv_1->setCurrentIndex(0);
    }
}

AddDialog::~AddDialog()
{
    delete ui;
}


void AddDialog::on_pb_add_clicked()
{
    switch(flag){
    case 0:server->addItem(Flight(ui->le_flight_0->text(), ui->sb_flight_0->value(),
                               ui->sb_flight_1->value(), ui->sb_flight_1->value(),
                               ui->le_flight_1->text(), ui->le_flight_2->text())); break;
    case 1:server->addItem(Hotel(ui->le_hotel_0->text(), ui->sb_hotel_0->value(),
                              ui->sb_hotel_1->value(), ui->sb_hotel_1->value())); break;
    case 2:server->addItem(Bus(ui->le_bus_0->text(), ui->sb_bus_0->value(),
                            ui->sb_bus_1->value(), ui->sb_bus_1->value())); break;
    case 3:server->addItem(User(ui->le_user_0->text(), ui->le_user_1->text(),
                             ui->le_user_2->text(), User::USER)); break;
    case 4:server->addItem(Reservation(ui->cb_resv_0->currentText(),
                                    ui->cb_resv_1->currentIndex(),
                                    ui->cb_resv_2->currentText())); break;
    }
    this->close();
}


void AddDialog::on_pb_cancel_clicked()
{
    this->close();
}


void AddDialog::on_cb_resv_1_currentIndexChanged(int index)
{
    ui->cb_resv_2->clear();
    bool ok;
    switch(index){
    case 0:{
        QList<QHash<QString, QString>> flights = server->getData(index);
        for(auto flight : flights){
            if(flight["numAvail"].toInt(&ok, 10) > 0){
                ui->cb_resv_2->addItem(flight["flightNum"]);
            }
        }
    }
    case 1:{
        QList<QHash<QString, QString>> hotels = server->getData(index);
        for(auto hotel : hotels){
            if(hotel["numAvail"].toInt(&ok, 10) > 0){
                ui->cb_resv_2->addItem(hotel["location"]);
            }
        }
    }
    case 2:{
        QList<QHash<QString, QString>> bus = server->getData(index);
        for(auto b : bus){
            if(b["numAvail"].toInt(&ok, 10) > 0){
                ui->cb_resv_2->addItem(b["location"]);
            }
        }
    }
    }
}


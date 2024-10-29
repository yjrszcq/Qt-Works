#include "myresvdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>

MyResvDialog::MyResvDialog(Flight flight){
    this->setWindowTitle("预订内容-航班");

    pb_ok = new QPushButton("确认");

    l_0 = new QLabel("航班号");
    l_1 = new QLabel("价格");
    l_2 = new QLabel("出发地");
    l_3 = new QLabel("到达地");

    le_0 = new QLineEdit();
    le_1 = new QLineEdit();
    le_2 = new QLineEdit();
    le_3 = new QLineEdit();

    le_0->setText(flight.getFlightNum());
    le_1->setText(QString::number(flight.getPrice()));
    le_2->setText(flight.getFromCity());
    le_3->setText(flight.getArivCity());

    le_0->setReadOnly(true);
    le_1->setReadOnly(true);
    le_2->setReadOnly(true);
    le_3->setReadOnly(true);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(l_0, 0, 0);
    grid->addWidget(le_0, 0, 1);
    grid->addWidget(l_1, 1, 0);
    grid->addWidget(le_1, 1, 1);
    grid->addWidget(l_2, 2, 0);
    grid->addWidget(le_2, 2, 1);
    grid->addWidget(l_3,3, 0);
    grid->addWidget(le_3, 3, 1);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addWidget(pb_ok);

    this->setLayout(vbox);

    connect(pb_ok, SIGNAL(clicked(bool)), this, SLOT(close()));
}

MyResvDialog::MyResvDialog(Hotel hotel){
    this->setWindowTitle("预订内容-宾馆");

    pb_ok = new QPushButton("确认");

    l_0= new QLabel("所在地");
    l_1 = new QLabel("价格");

    le_0 = new QLineEdit();
    le_1 = new QLineEdit();

    le_0->setText(hotel.getLocation());
    le_1->setText(QString::number(hotel.getPrice()));

    le_0->setReadOnly(true);
    le_1->setReadOnly(true);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(l_0, 0, 0);
    grid->addWidget(le_0, 0, 1);
    grid->addWidget(l_1, 1, 0);
    grid->addWidget(le_1, 1, 1);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addWidget(pb_ok);

    this->setLayout(vbox);

    connect(pb_ok, SIGNAL(clicked(bool)), this, SLOT(close()));
}

MyResvDialog::MyResvDialog(Bus bus){
    this->setWindowTitle("预订内容-客车");

    pb_ok = new QPushButton("确认");

    l_0= new QLabel("所在地");
    l_1 = new QLabel("价格");

    le_0 = new QLineEdit();
    le_1 = new QLineEdit();

    le_0->setText(bus.getLocation());
    le_1->setText(QString::number(bus.getPrice()));

    le_0->setReadOnly(true);
    le_1->setReadOnly(true);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(l_0, 0, 0);
    grid->addWidget(le_0, 0, 1);
    grid->addWidget(l_1, 1, 0);
    grid->addWidget(le_1, 1, 1);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addWidget(pb_ok);

    this->setLayout(vbox);

    connect(pb_ok, SIGNAL(clicked(bool)), this, SLOT(close()));

}

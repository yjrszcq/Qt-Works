#include "setdbdialog.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>

SetDbDialog::SetDbDialog(QString host, int port, QString user, QString password, QString database) {
    *l = new QLabel[5];
    l[0] = new QLabel("host");
    l[1] = new QLabel("port");
    l[2] = new QLabel("user");
    l[3] = new QLabel("password");
    l[4] = new QLabel("database");

    *le = new QLineEdit[5];
    le[0] = new QLineEdit(host);
    le[1] = new QLineEdit(QString::number(port));
    le[2] = new QLineEdit(user);
    le[3] = new QLineEdit(password);
    le[4] = new QLineEdit(database);


    pb_yes = new QPushButton("确认");
    pb_no = new QPushButton("取消");

    QGridLayout *grid = new QGridLayout();
    for(int i = 0; i < 5; i ++){
        grid->addWidget(l[i], i, 0);
        grid->addWidget(le[i], i, 1);
    }

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(pb_yes);
    hbox->addWidget(pb_no);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addLayout(hbox);

    this->setLayout(vbox);

    connect(pb_yes, SIGNAL(clicked(bool)), this, SLOT(pbYes()));
    connect(pb_no, SIGNAL(clicked(bool)), this, SLOT(pbNo()));
}

void SetDbDialog::pbYes(){
    emit dataSent(le[0]->text(), le[1]->text().toInt(), le[2]->text(), le[3]->text(), le[4]->text(), true);
    this->close();
}

void SetDbDialog::pbNo(){
    emit dataSent(le[0]->text(), le[1]->text().toInt(), le[2]->text(), le[3]->text(), le[4]->text(), false);
    this->close();
}

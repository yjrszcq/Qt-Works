#include "setdbdialog.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSettings>

SetDbDialog::SetDbDialog(QString config_file_path) {
    this->setWindowTitle("设置数据库连接");
    this->config_file_path = config_file_path;

    *l = new QLabel[5];
    l[0] = new QLabel("host");
    l[1] = new QLabel("port");
    l[2] = new QLabel("user");
    l[3] = new QLabel("password");
    l[4] = new QLabel("database");

    QSettings config(config_file_path, QSettings::IniFormat);
    *le = new QLineEdit[5];
    le[0] = new QLineEdit(config.value("Database/host").toString());
    le[1] = new QLineEdit(config.value("Database/port").toString());
    le[2] = new QLineEdit(config.value("Database/user").toString());
    le[3] = new QLineEdit(config.value("Database/password").toString());
    le[4] = new QLineEdit(config.value("Database/database").toString());

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
    QSettings *config = new QSettings(config_file_path, QSettings::IniFormat);
    config->setValue("Database/host", le[0]->text());
    config->setValue("Database/port", le[1]->text().toInt());
    config->setValue("Database/user", le[2]->text());
    config->setValue("Database/password", le[3]->text());
    config->setValue("Database/database", le[4]->text());
    delete config;
    emit dbSetSent(true);
    this->close();
}

void SetDbDialog::pbNo(){
    emit dbSetSent(false);
    this->close();
}

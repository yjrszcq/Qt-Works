#include "signupdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>

SignupDialog::SignupDialog(Servor *servor) {
    this->setWindowTitle("注册");
    this->servor = servor;

    l_name = new QLabel("用户名");
    l_id = new QLabel("ID");

    le_name = new QLineEdit();
    le_id = new QLineEdit();

    pb_signup = new QPushButton("注册");

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(l_name, 0, 0);
    grid->addWidget(le_name, 0, 1);
    grid->addWidget(l_id, 1, 0);
    grid->addWidget(le_id, 1, 1);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addWidget(pb_signup);

    this->setLayout(vbox);

    connect(pb_signup, SIGNAL(clicked(bool)), this, SLOT(pbSignup()));
}

void SignupDialog::pbSignup(){
    servor->signup(le_name->text(), le_id->text());
    this->close();
}

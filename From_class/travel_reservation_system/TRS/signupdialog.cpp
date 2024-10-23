#include "signupdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>

SignupDialog::SignupDialog(Server *server) {
    this->setWindowTitle("注册");
    this->server = server;

    l_name = new QLabel("用户名");
    l_id = new QLabel("ID");
    l_password = new QLabel("密码");

    le_name = new QLineEdit();
    le_id = new QLineEdit();
    le_password = new QLineEdit();

    pb_signup = new QPushButton("注册");

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(l_name, 0, 0);
    grid->addWidget(le_name, 0, 1);
    grid->addWidget(l_id, 1, 0);
    grid->addWidget(le_id, 1, 1);
    grid->addWidget(l_password, 2, 0);
    grid->addWidget(le_password, 2, 1);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addWidget(pb_signup);

    this->setLayout(vbox);

    connect(le_name, SIGNAL(textChanged(QString)), le_id, SLOT(setText(QString)));
    connect(pb_signup, SIGNAL(clicked(bool)), this, SLOT(pbSignup()));
}

void SignupDialog::pbSignup(){
    server->signup(User(le_name->text(), le_id->text(), le_password->text(), User::USER));
    this->close();
}

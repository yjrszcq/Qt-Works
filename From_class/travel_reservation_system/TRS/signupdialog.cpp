#include "signupdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>

SignupDialog::SignupDialog(Server *server) {
    this->setWindowTitle("注册");
    this->server = server;

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
    server->signup(User(le_name->text(), le_id->text(), User::USER));
    this->close();
}

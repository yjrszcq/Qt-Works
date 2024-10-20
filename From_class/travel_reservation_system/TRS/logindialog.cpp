#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

LoginDialog::LoginDialog(Server *server) {
    this->setWindowTitle("登录");
    this->server = server;

    l_option = new QLabel("身份");
    l_name = new QLabel("用户名");
    l_id = new QLabel("ID");

    le_name = new QLineEdit(this);
    le_id = new QLineEdit(this);

    rb_user = new QRadioButton("用户");
    rb_root = new QRadioButton("管理员");

    pb_login = new QPushButton("登录");

    rb_user->setChecked(true);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(l_name, 0, 0);
    grid->addWidget(le_name, 0, 1);
    grid->addWidget(l_id, 1, 0);
    grid->addWidget(le_id, 1, 1);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(l_option);
    hbox->addWidget(rb_user);
    hbox->addWidget(rb_root);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addLayout(hbox);
    vbox->addWidget(pb_login);

    this->setLayout(vbox);

    connect(rb_root, SIGNAL(clicked(bool)), this, SLOT(nameHide()));
    connect(rb_user, SIGNAL(clicked(bool)), this, SLOT(nameShow()));
    connect(pb_login, SIGNAL(clicked(bool)), this, SLOT(pbLogin()));
}

void LoginDialog::nameHide(){
    l_name->hide();
    le_name->hide();
}

void LoginDialog::nameShow(){
    l_name->show();
    le_name->show();
}

void LoginDialog::pbLogin(){
    bool jud = false;
    if(rb_user->isChecked() == User::USER){
        jud = server->login(User(le_name->text(), le_id->text(), User::USER));
    } else{
        jud = server->login(User(User::ROOT));
    }
    if(jud){
        this->close();
    } else{
        le_name->setText("");
        le_id->setText("");
    }
}


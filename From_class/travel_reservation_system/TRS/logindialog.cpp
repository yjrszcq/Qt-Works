#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

LoginDialog::LoginDialog(Servor *servor) {
    this->setWindowTitle("登录");
    this->servor = servor;

    l_option = new QLabel("身份");
    l_name = new QLabel("姓名");
    l_id = new QLabel("ID");

    le_name = new QLineEdit(this);
    le_id = new QLineEdit(this);

    rb_user = new QRadioButton("用户");
    rb_root = new QRadioButton("管理员");

    pb_login = new QPushButton("登录");

    rb_user->setChecked(true);

    QHBoxLayout *hb_name = new QHBoxLayout();
    hb_name->addWidget(l_name);
    hb_name->addWidget(le_name);

    QHBoxLayout *hb_id = new QHBoxLayout();
    hb_id->addWidget(l_id);
    hb_id->addWidget(le_id);

    QHBoxLayout *hb_option = new QHBoxLayout();
    hb_option->addWidget(l_option);
    hb_option->addWidget(rb_user);
    hb_option->addWidget(rb_root);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(hb_name);
    vbox->addLayout(hb_id);
    vbox->addLayout(hb_option);
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
        jud = servor->login(le_name->text(), le_id->text(), User::USER);
    } else{
        jud = servor->login("root", le_id->text(), User::ROOT);
    }
    if(jud){
        this->close();
    } else{
        le_name->setText("");
        le_id->setText("");
    }
}


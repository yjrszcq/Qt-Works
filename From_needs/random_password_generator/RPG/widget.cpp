#include "widget.h"
#include <bits/stdc++.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include "historydialog.h"

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("随机密码生成器");
    this->setWindowIcon(QIcon("SZCQ.png"));
    this->setFixedWidth(280);
    this->setFixedHeight(125);

    le_length = new QLineEdit("16");

    cb_mark = new QComboBox();
    cb_mark->addItem("不添加符号");
    cb_mark->addItem("添加常用符号");
    cb_mark->addItem("添加所有符号");

    le_password = new QLineEdit();

    pb_confirm = new QPushButton("生成密码");
    pb_history = new QPushButton("保存密码");

    QFormLayout *form = new QFormLayout();
    form->addRow("密码长度", le_length);
    form->addRow("设置符号", cb_mark);
    form->addRow("密码显示", le_password);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(pb_confirm);
    hbox->addWidget(pb_history);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addLayout(form);
    vbox->addLayout(hbox);

    this->setLayout(vbox);

    set_length();
    set_mark(0);

    connect(cb_mark, SIGNAL(currentIndexChanged(int)), this, SLOT(set_mark(int)));
    connect(pb_confirm, SIGNAL(clicked(bool)), this, SLOT(set_length()));
    connect(pb_confirm, SIGNAL(clicked(bool)), this, SLOT(generate_password()));
    connect(pb_history, SIGNAL(clicked(bool)), this, SLOT(generate_history()));
}

Widget::~Widget() {}

void Widget::set_length(){
    string length_str = le_length->text().toStdString();
    if(length_str == ""){
        this->length = 0;
    } else{
        this->length = stoi(length_str);
    }
}

void Widget::set_mark(int jud){
    string library_base = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string library_common = "~!@#$%^&*()_+";
    string library_all = "`~!@#$%^&*()-=[]\\;',./_+{}|:\"<>?";
    switch(jud){
    case 0: this->library = library_base; break;
    case 1: this->library = library_base + library_common; break;
    case 2: this->library = library_base + library_all; break;
    default: break;
    }
}

void Widget::generate_password(){
    if(this->length <= 0){
        le_password->setText("请输入正确的密码长度！");
    } else{
        srand(time(0));
        string password;
        for (int i = 0; i < this->length; i++) {
            int key = rand() % library.length();
            password += library[key];
        }
        QString password_result = QString::fromStdString(password);
        le_password->setText(password_result);
    }
}

void Widget::generate_history(){
    HistoryDialog::show(le_password->text());
}

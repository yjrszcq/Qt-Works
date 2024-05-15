#include "historydialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

HistoryDialog::HistoryDialog(QString password, std::vector<HistoryDialog::Info> infos) {
    this->infos = infos;
    this->password = password;

    cb_choice = new QComboBox();
    cb_choice->addItem("添加密码");
    for(auto info : this->infos){
        cb_choice->addItem(info.website);
    }
    le_website = new QLineEdit();
    le_username = new QLineEdit();
    le_password = new QLineEdit();

    pb_save = new QPushButton("保存");
    pb_delete = new QPushButton("删除");

    QFormLayout *form = new QFormLayout();
    form->addRow("网址", le_website);
    form->addRow("用户", le_username);
    form->addRow("密码", le_password);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(pb_save);
    hbox->addWidget(pb_delete);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(cb_choice);
    vbox->addLayout(form);
    vbox->addLayout(hbox);

    setLayout(vbox);

    set_info(0);
    cb_choice->currentIndexChanged(0);

    connect(cb_choice, SIGNAL(currentIndexChanged(int)), this, SLOT(set_info(int)));
    connect(pb_save, SIGNAL(clicked(bool)), this, SLOT(save_info()));
    connect(pb_delete, SIGNAL(clicked(bool)), this, SLOT(delete_info()));

}

void HistoryDialog::set_info(int index){
    if(index == 0){
        le_website->setText("");
        le_username->setText("");
        le_password->setText(this->password);
    } else{
        le_website->setText(this->infos[index - 1].website);
        le_username->setText(this->infos[index - 1].username);
        le_password->setText(this->infos[index - 1].password);
    }
}

void HistoryDialog::save_info(){
    int index = cb_choice->currentIndex();
    if(index == 0){
        Info info;
        info.website = le_website->text();
        info.username = le_username->text();
        info.password = le_password->text();
        if(info.password != ""){
            this->infos.push_back(info);
            save_data();
            close();
        }
    } else{
        this->infos[index - 1].website = le_website->text();
        this->infos[index - 1].username = le_username->text();
        this->infos[index - 1].password = le_password->text();
        save_data();
        close();
    }
}

void HistoryDialog::delete_info(){
    int index = cb_choice->currentIndex();
    if(index == 0){
        QMessageBox::information(this, "错误", "请选择任意一项删除", "确认");
    } else{
        this->infos.erase(this->infos.begin() + index - 1);
        save_data();
        close();
    }
}

void HistoryDialog::save_data(){
    QMessageBox::information(this, "成功", "保存/删除成功", "确认");
}

#include "historydialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <algorithm>
#include <fstream>
#include <io.h>

HistoryDialog::HistoryDialog(QString password) {
    this->setWindowTitle("密码管理");
    this->setWindowIcon(QIcon("SZCQ.png"));
    this->password = password;

    loadData();

    cb_choice = new QComboBox();
    cb_choice->addItem("添加密码");
    int i = 1;
    for(auto info : infos){
        if(info.getWebsite() != ""){
            cb_choice->addItem(info.getWebsite());
        } else{
            cb_choice->addItem(QString::fromStdString("unknown_" + std::to_string(i)));
            i ++;
        }
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

    this->setLayout(vbox);

    setInfo(0);
    cb_choice->currentIndexChanged(0);

    connect(cb_choice, SIGNAL(currentIndexChanged(int)), this, SLOT(setInfo(int)));
    connect(pb_save, SIGNAL(clicked(bool)), this, SLOT(saveInfo()));
    connect(pb_delete, SIGNAL(clicked(bool)), this, SLOT(deleteInfo()));

}

void HistoryDialog::setInfo(int index){
    if(index == 0){
        le_website->setText("");
        le_username->setText("");
        le_password->setText(password);
    } else{
        le_website->setText(infos[index - 1].getWebsite());
        le_username->setText(infos[index - 1].getUsername());
        le_password->setText(infos[index - 1].getPassword());
    }
}

void HistoryDialog::saveInfo(){
    int index = cb_choice->currentIndex();
    if(index == 0){
        if(le_password->text() != ""){
            bool jud = true;
            for(auto info : infos){
                if(info.getWebsite() == le_website->text()){
                    if(info.getUsername() == le_username->text()){
                        if(info.getPassword() == le_password->text()){
                            jud = false;
                        }
                    }
                }
            }
            if(jud == true){
                PasswordInfo info(le_website->text(), le_username->text(), le_password->text());
                this->infos.push_back(info);
                saveData(1);
                close();
            } else{
                QMessageBox::information(this, "错误", "数据重复", "确认");
            }
        } else{
            QMessageBox::information(this, "错误", "密码不能为空", "确认");
        }
    } else{
        if(le_password->text() != ""){
            this->infos[index - 1].setWebsite(le_website->text());
            this->infos[index - 1].setUsernmae(le_username->text());
            this->infos[index - 1].setPassword(le_password->text());
            saveData(1);
            close();
        } else{
            QMessageBox::information(this, "错误", "密码不能为空", "确认");
        }
    }
}

void HistoryDialog::deleteInfo(){
    int index = cb_choice->currentIndex();
    if(index == 0){
        QMessageBox::information(this, "错误", "请选择任意一项删除", "确认");
    } else{
        infos.erase(infos.begin() + index - 1);
        saveData(0);
        close();
    }
}

void HistoryDialog::loadData(){
    if(access("data", 0) != 0){
        mkdir("data");
        return;
    }
    std::ifstream ifs("data/password_infos.csv");
    if(!ifs){
        return;
    }
    std::string line;
    while(getline(ifs, line)){
        std::vector<QString> datas = PasswordInfo::inputTransform(QString::fromStdString(line));
        PasswordInfo info(datas[0], datas[1], datas[2]);
        infos.push_back(info);
    }
    ifs.close();
    sort(infos.begin(), infos.end(), [](const PasswordInfo& p1, const PasswordInfo& p2) {
        if (p1.getWebsite() == p2.getWebsite()) {
            return p1.getUsername() < p2.getUsername();
        }
        return p1.getWebsite() < p2.getWebsite();
    });
    int i = 0;
    while(infos[0].getWebsite() == ""){
        if(i == infos.size()){
            break;
        }
        infos.push_back(infos[0]);
        infos.erase(infos.begin());
        i ++;
    }
}

void HistoryDialog::saveData(int jud){
    if(access("data/password_infos.csv", 0) == 0 && infos.empty()){
        remove("data/password_infos.csv");
        saveMessage(jud);
        close();
    } else{
        std::ofstream ofs("data/password_infos.csv");
        for(auto info : infos){
            std::vector<QString> datas = {info.getWebsite(), info.getUsername(), info.getPassword()};
            ofs << PasswordInfo::outputTransform(datas).toStdString() << std::endl;
        }
        ofs.close();
        saveMessage(jud);
    }
}

void HistoryDialog::saveMessage(int jud){
    if(jud == 0){
        QMessageBox::information(this, "成功", "删除成功", "确认");
    } else if(jud == 1){
        QMessageBox::information(this, "成功", "保存成功", "确认");
    }
}

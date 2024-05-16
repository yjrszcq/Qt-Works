#include "historydialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <bits/stdc++.h>
#include <io.h>

using namespace std;

HistoryDialog::HistoryDialog(QString password) {
    this->setWindowTitle("密码管理");
    this->setWindowIcon(QIcon("SZCQ.png"));
    load_data();
    this->password = password;
    cb_choice = new QComboBox();
    cb_choice->addItem("添加密码");
    int i = 0;
    for(auto info : this->infos){
        if(info.get_website() != ""){
            cb_choice->addItem(info.get_website());
        } else{
            cb_choice->addItem(QString::fromStdString("null_" + to_string(i)));
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
        le_website->setText(this->infos[index - 1].get_website());
        le_username->setText(this->infos[index - 1].get_username());
        le_password->setText(this->infos[index - 1].get_password());
    }
}

void HistoryDialog::save_info(){
    int index = cb_choice->currentIndex();
    if(index == 0){
        if(le_password->text() != ""){
            bool jud = true;
            for(auto info : infos){
                if(info.get_website() == le_website->text()){
                    if(info.get_username() == le_username->text()){
                        if(info.get_password() == le_password->text()){
                            jud = false;
                        }
                    }
                }
            }
            if(jud == true){
                PasswordInfo info(le_website->text(), le_username->text(), le_password->text());
                this->infos.push_back(info);
                save_data(1);
                close();
            } else{
                QMessageBox::information(this, "错误", "数据重复", "确认");
            }
        } else{
            QMessageBox::information(this, "错误", "密码不能为空", "确认");
        }
    } else{
        if(le_password->text() != ""){
            this->infos[index - 1].set_website(le_website->text());
            this->infos[index - 1].set_usernmae(le_username->text());
            this->infos[index - 1].set_password(le_password->text());
            save_data(1);
            close();
        } else{
            QMessageBox::information(this, "错误", "密码不能为空", "确认");
        }
    }
}

void HistoryDialog::delete_info(){
    int index = cb_choice->currentIndex();
    if(index == 0){
        QMessageBox::information(this, "错误", "请选择任意一项删除", "确认");
    } else{
        this->infos.erase(this->infos.begin() + index - 1);
        save_data(0);
        close();
    }
}

void HistoryDialog::load_data(){
    if(access("data", 0) != 0){
        mkdir("data");
        return;
    }
    ifstream ifs("data/password_infos.csv");
    if(!ifs){
        return;
    }
    string line;
    while(getline(ifs, line)){
        istringstream is(line);
        vector<string> parts;
        string temp;
        while(getline(is, temp, ',')){
            parts.push_back(temp);
        }
        for(int i = 0; i < parts.size(); i ++){
            int num = 0;
            for(int j = 0; j < parts[i].length(); j ++){
                if(parts[i][j] == '"'){
                    num ++;
                }
            }
            if(num %2 == 1){
                parts[i] += "," + parts[i + 1];
                parts.erase(parts.begin() + i + 1);
                i --;
                continue;
            } else{
                parts[i].erase(parts[i].end() - 1);
                parts[i].erase(parts[i].begin());
                for(int j = 0; j < parts[i].length(); j ++){
                    if(parts[i][j] == '"' && parts[i][j + 1] == '"'){
                        parts[i].erase(parts[i].begin() + j);
                    }
                }
            }
        }
        PasswordInfo info(QString::fromStdString(parts[0]), QString::fromStdString(parts[1]), QString::fromStdString(parts[2]));
        infos.push_back(info);
    }
    ifs.close();
    sort(infos.begin(), infos.end(), [](const PasswordInfo& p1, const PasswordInfo& p2) {
        if (p1.get_website() == p2.get_website()) {
            return p1.get_username() < p2.get_username();
        }
        return p1.get_website() < p2.get_website();
    });
    int i = 0;
    while(infos[0].get_website() == ""){
        if(i == infos.size()){
            break;
        }
        infos.push_back(infos[0]);
        infos.erase(infos.begin());
        i ++;
    }
}

void HistoryDialog::save_data(int jud){
    if(access("data/password_infos.csv", 0) == 0 && infos.empty()){
        remove("data/password_infos.csv");
        save_message(jud);
        close();
    } else{
        ofstream ofs("data/password_infos.csv");
        for(auto info : infos){
            ofs << transform(info.get_website().toStdString()) << ",";
            ofs << transform(info.get_username().toStdString()) << ",";
            ofs << transform(info.get_password().toStdString()) << endl;
        }
        ofs.close();
        save_message(jud);
    }
}

string HistoryDialog::transform(string str){
    for(int i = 0; i < str.length(); i ++){
        if(str[i] == '"'){
            str.insert(i, 1, '"');
            i ++;
        }
    }
    return "\"" + str + "\"";
}

void HistoryDialog::save_message(int jud){
    if(jud == 1){
        QMessageBox::information(this, "成功", "保存成功", "确认");
    } else if(jud == 0){
        QMessageBox::information(this, "成功", "删除成功", "确认");
    }
}

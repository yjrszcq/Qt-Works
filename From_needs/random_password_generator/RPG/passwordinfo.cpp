#include "passwordinfo.h"
#include <sstream>

PasswordInfo::PasswordInfo(QString website, QString username, QString password) {
    this->website = website;
    this->username = username;
    this->password = password;
}

void PasswordInfo::setWebsite(QString website){
    this->website = website;
}

void PasswordInfo::setUsernmae(QString username){
    this->username = username;
}

void PasswordInfo::setPassword(QString password){
    this->password = password;
}

QString PasswordInfo::getWebsite() const{
    return website;
}

QString PasswordInfo::getUsername() const{
    return username;
}

QString PasswordInfo::getPassword() const{
    return password;
}

std::vector<QString> PasswordInfo::inputTransform(QString line){
    std::istringstream iss(line.toStdString());
    std::vector<std::string> parts;
    std::string temp;
    while(getline(iss, temp, ',')){
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
            if(i == parts.size() - 1){
                return {};
            }
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
    std::vector<QString> datas;
    for(auto part : parts){
        datas.push_back(QString::fromStdString(part));
    }
    return datas;
}

QString PasswordInfo::outputTransform(std::vector<QString> datas){
    QString line;
    for(auto data : datas){
        line += outputTransformSingle(data);
        if(data != datas.back()){
            line += ",";
        }
    }
    return line;
}

QString PasswordInfo::outputTransformSingle(QString data){
    std::string str = data.toStdString();
    for(int i = 0; i < str.length(); i ++){
        if(str[i] == '"'){
            str.insert(i, 1, '"');
            i ++;
        }
    }
    return QString::fromStdString("\"" + str + "\"");
}

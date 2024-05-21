#include "passwordinfo.h"
#include <sstream>
#include "crypt.h"

using namespace Crypt;

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

bool PasswordInfo::is_ascii(QString qstr){
    std::string str = qstr.toStdString();
    for(auto c : str){
        if(!isascii(c)){
            return false;
        }
    }
    return true;
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
    if(parts.size() == 3){
        std::vector<QString> datas;
        if(parts[2][0] != '='){
            if(parts[1] != "" && is_ascii(QString::fromStdString(parts[1]))){
                parts[2] = decrypt_password_base32_xor(parts[2], parts[1]);
            } else{
                parts[2] = decrypt_password_base32_xor(parts[2], "username");
            }
        } else{
            parts[2].erase(parts[2].begin());
            if(parts[1] != "" && is_ascii(QString::fromStdString(parts[1]))){
                parts[2] = decrypt_password_base32_xor(parts[2], parts[1]);
            } else{
                parts[2] = decrypt_password_base32_xor(parts[2], "username");
            }
        }
        parts[2].erase(parts[2].begin() + parts[2].length() - 1);
        for(auto part : parts){
            datas.push_back(QString::fromStdString(part));
        }
        return datas;
    } else{
        return {};
    }
}

QString PasswordInfo::outputTransform(std::vector<QString> datas){
    QString line;
    line += outputTransformSingle(datas[0], false, "") + ",";
    line += outputTransformSingle(datas[1], false, "") + ",";
    line += outputTransformSingle(datas[2], true, datas[1]);
    return line;
}

QString PasswordInfo::outputTransformSingle(QString data, bool jud, QString username){
    std::string str;
    if(jud == true ){
        if(is_ascii(data)){
            if(username != "" && is_ascii(username)){
                str = encrypt_password_base32_xor(data.toStdString() + "+", username.toStdString());
            } else{
                str = encrypt_password_base32_xor(data.toStdString() + "+", "username");
            }
        } else{
            if(username != "" && is_ascii(username)){
                str = "=" + encrypt_password_base32_xor(data.toStdString() + "+", username.toStdString());
            } else{
                str = "=" + encrypt_password_base32_xor(data.toStdString() + "+", "username");
            }
        }
    } else{
        str = data.toStdString();
    }
    for(int i = 0; i < str.length(); i ++){
        if(str[i] == '"'){
            str.insert(i, 1, '"');
            i ++;
        }
    }
    return QString::fromStdString("\"" + str + "\"");
}


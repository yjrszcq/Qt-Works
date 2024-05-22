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
        if(parts[2][0] == '='){ //根据标记判断密码是否不完全由ascii里的字符组成
            parts[2].erase(parts[2].begin());
        }
        if(parts[1] != ""){ //判断名称是否为空
            if(is_ascii(QString::fromStdString(parts[1]))){ //判断名称是否由ascii里的字符组成
                parts[2] = decrypt_password_base32_xor(parts[2], parts[1]);
            } else{ //若不是则先将名称用md5转化
                parts[2] = decrypt_password_base32_xor(parts[2], username_to_md5_to_str(parts[1]));
            }
        } else{ //若为空则用默认key解密
            parts[2] = decrypt_password_base32_xor(parts[2], "username");
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
    if(jud == true ){ //判断是否是密码，即datas[2]
        if(is_ascii(data)){ //判断密码是否是由ascii里的字符组成
            str = "";
        } else{ //若不是则加'='用于识别，为将来中文名单独更换算法作准备
            str = "=";
        }
        if(username != ""){ //判断名称是否为空
            if(is_ascii(username)){ //判断名称是否由ascii里的字符组成
                str += encrypt_password_base32_xor(data.toStdString() + "+", username.toStdString());
            } else{ //若不是则先将名称用md5转化
                str += encrypt_password_base32_xor(data.toStdString() + "+", username_to_md5_to_str(username.toStdString()));
            }
        } else{ //若为空则用默认key加密
            str += encrypt_password_base32_xor(data.toStdString() + "+", "username");
        }
    } else{ //若不是则直接录入
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


#include "passwordinfo.h"

PasswordInfo::PasswordInfo(QString website, QString username, QString password) {
    this->website = website;
    this->username = username;
    this->password = password;
}

void PasswordInfo::set_website(QString website){
    this->website = website;
}

void PasswordInfo::set_usernmae(QString username){
    this->username = username;
}

void PasswordInfo::set_password(QString password){
    this->password = password;
}

QString PasswordInfo::get_website() const{
    return website;
}

QString PasswordInfo::get_username() const{
    return username;
}

QString PasswordInfo::get_password() const{
    return password;
}

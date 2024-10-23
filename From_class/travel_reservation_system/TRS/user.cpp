#include "user.h"

User::User(){
    name = "未登录";
    id = "";
    password = "";
    this->permission = VISITOR;
}
User::User(Permission permission){ // 构造函数
    name = "";
    id = "";
    password = "";
    this->permission = permission;
    if(permission == User::VISITOR){
        name = "未登录";
        id = "未登录";
    } else if(permission == User::ROOT){
        name = "root";
        id = "root";
    }
}

User::User(QString name, QString id, QString password, Permission permission){
    this->name = name;
    this->id = id;
    this->password = password;
    this->permission = permission;
}

void User::setName(QString name){ // 设置用户名
    this->name = name;
}

void User::setId(QString id){ // 设置ID
    this->id = id;
}

void User::setPassword(QString password){ // 设置密码
    this->password = password;
}

void User::setPermission(Permission permission){ // 设置权限
    this->permission = permission;
}

QString User::getName(){ // 获取用户名
    return name;
}

QString User::getId(){ // 获取ID
    return id;
}

QString User::getPassword(){ // 获取密码
    return password;
}

User::Permission User::getPermission(){ // 获取权限
    return permission;
}

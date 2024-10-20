#include "user.h"

User::User(){
    name = "未登录";
    id = "";
    this->permission = VISITOR;
}
User::User(Permission permission){ // 构造函数
    name = "";
    id = "";
    this->permission = permission;
    if(permission == User::VISITOR){
        name = "未登录";
    } else if(permission == User::ROOT){
        name = "root";
        id = "1234";
    }
}

User::User(QString name, QString id, Permission permission){
    this->name = name;
    this->id = id;
    this->permission = permission;
}

void User::setName(QString name){ // 设置用户名
    this->name = name;
}

void User::setId(QString id){ // 设置密码
    this->id = id;
}

void User::setPermission(Permission permission){ // 设置权限
    this->permission = permission;
}

QString User::getName(){ // 获取用户名
    return name;
}

QString User::getId(){ // 获取密码
    return id;
}

User::Permission User::getPermission(){ // 获取权限
    return permission;
}

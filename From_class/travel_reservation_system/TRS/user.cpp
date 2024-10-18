#include "user.h"

User::User(){
    name = "";
    id = "";
    this->permission = VISITOR;
}
User::User(Permission permission){ // 构造函数
    name = "";
    id = "";
    this->permission = permission;
}

User::User(QString name, QString id, Permission permission){
    this->name = name;
    this->id = id;
    this->permission = permission;
}

void User::set_name(QString name){ // 设置用户名
    this->name = name;
}

void User::set_id(QString id){ // 设置密码
    this->id = id;
}

void User::set_permission(Permission permission){ // 设置权限
    this->permission = permission;
}

QString User::get_name(){ // 获取用户名
    return name;
}

QString User::get_id(){ // 获取密码
    return id;
}

User::Permission User::get_permission(){ // 获取权限
    return permission;
}

#ifndef USER_H
#define USER_H
#include <QString>

class User
{ // 用户类
public:
    enum Permission{VISITOR, USER, ROOT}; // 权限枚举
    User();
    User(Permission permission); // 构造函数
    User(QString name, QString id, Permission permission);
    void setName(QString name); // 设置用户名
    void setId(QString id); // 设置密码
    void setPermission(Permission permission = USER); // 设置权限
    QString getName(); // 获取用户名
    QString getId(); // 获取密码
    Permission getPermission(); // 获取权限
private:
    QString name; // 用户名
    QString id; // 密码
    Permission permission; // 权限
};

#endif // USER_H

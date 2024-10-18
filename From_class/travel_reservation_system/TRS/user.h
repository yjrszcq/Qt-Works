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
    void set_name(QString name); // 设置用户名
    void set_id(QString id); // 设置密码
    void set_permission(Permission permission = USER); // 设置权限
    QString get_name(); // 获取用户名
    QString get_id(); // 获取密码
    Permission get_permission(); // 获取权限
private:
    QString name; // 用户名
    QString id; // 密码
    Permission permission; // 权限
};

#endif // USER_H

#ifndef PASSWORDINFO_H
#define PASSWORDINFO_H
#include <QString>

class PasswordInfo
{
public:
    PasswordInfo(QString, QString, QString);
    void set_website(QString);
    void set_usernmae(QString);
    void set_password(QString);
    QString get_website() const;
    QString get_username() const;
    QString get_password() const;
private:
    QString website;
    QString username;
    QString password;
};

#endif // PASSWORDINFO_H

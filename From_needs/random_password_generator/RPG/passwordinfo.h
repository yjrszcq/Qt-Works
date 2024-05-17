#ifndef PASSWORDINFO_H
#define PASSWORDINFO_H
#include <QString>

class PasswordInfo
{
public:
    PasswordInfo(QString, QString, QString);
    void setWebsite(QString);
    void setUsernmae(QString);
    void setPassword(QString);
    QString getWebsite() const;
    QString getUsername() const;
    QString getPassword() const;
    static std::vector<QString> inputTransform(QString line);
    static QString outputTransform(std::vector<QString> datas);
    static QString outputTransformSingle(QString data);
private:
    QString website;
    QString username;
    QString password;
};

#endif // PASSWORDINFO_H

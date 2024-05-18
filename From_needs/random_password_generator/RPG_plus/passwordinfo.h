#ifndef PASSWORDINFO_H
#define PASSWORDINFO_H
#include <QString>

class PasswordInfo
{
public:
    PasswordInfo(QString website, QString username, QString password);
    void setWebsite(QString website);
    void setUsernmae(QString username);
    void setPassword(QString password);
    QString getWebsite() const;
    QString getUsername() const;
    QString getPassword() const;
    static std::vector<QString> inputTransform(QString line);
    static QString outputTransform(std::vector<QString> datas);
    static QString outputTransformSingle(QString data, bool jud);
private:
    QString website;
    QString username;
    QString password;
};

#endif // PASSWORDINFO_H

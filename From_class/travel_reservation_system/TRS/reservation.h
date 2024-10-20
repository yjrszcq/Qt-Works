#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>

class Reservation{
public:
    Reservation(QString user_name);
    Reservation(QString user_name, int resv_type, QString resv_key);
    void setUserName(QString user_name);
    void setResvType(int resv_type);
    void setResvKey(QString resv_key);
    QString getUserName();
    int getResvType();
    QString getResvKey();

private:
    QString user_name;
    int resv_type;
    QString resv_key;
};

#endif // RESERVATION_H

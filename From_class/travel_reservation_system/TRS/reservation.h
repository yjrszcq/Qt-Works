#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>

class Reservation{
public:
    enum Type{FLIGHT, HOTEL, BUS};
    enum State{UNAVAILABLE, AVAILABLE};
    Reservation(QString user_name, int resv_type, QString resv_content);
    Reservation(QString user_name, int resv_type, QString resv_content, QString resv_key, bool resv_avail, QString resv_note);
    void setUserName(QString user_name);
    void setResvType(int resv_type);
    void setResvContent(QString resv_content);
    void setResvAvail(bool resv_avail);
    void setResvKey(QString resv_key);
    void setResvNote(QString resv_note);
    QString getUserName();
    int getResvType();
    QString getResvContent();
    bool getResvAvail();
    QString getResvKey();
    QString getResvNote();

private:
    QString user_name;
    int resv_type;
    QString resv_content;
    bool resv_avail;
    QString resv_key;
    QString resv_note;

private:
    void resvKeyGenerate();
};

#endif // RESERVATION_H

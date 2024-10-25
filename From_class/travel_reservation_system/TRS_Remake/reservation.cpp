#include "reservation.h"

#include <QDateTime>
#include <QCryptographicHash>
#include <QRandomGenerator>

Reservation::Reservation(QString resv_key, bool resv_avail){
    this->user_name = "";
    this->resv_type = 0;
    this->resv_content = "";
    this->resv_avail = resv_avail;
    this->resv_key = resv_key;
}

Reservation::Reservation(QString user_name, int resv_type, QString resv_content){
    this->user_name = user_name;
    this->resv_type = resv_type;
    this->resv_content = resv_content; // 各个表的主键内容
    this->resv_avail = Reservation::AVAILABLE;
    resvKeyGenerate();
}

void Reservation::resvKeyGenerate(){
    QString rk = "";
    switch(resv_type){
    case FLIGHT: rk += "F"; break;
    case HOTEL: rk += "H"; break;
    case BUS: rk += "B"; break;
    }
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(user_name.toUtf8());
    QString hashString = hash.result().toHex();
    for(int i = 0; i < 5; i ++){
        rk += hashString[QRandomGenerator::global()->bounded(1, 32)].toUpper();
    }
    QDateTime currentDateTime = QDateTime::currentDateTime();
    rk += QString::number(currentDateTime.date().year());
    rk += QString::number(currentDateTime.date().month());
    rk += QString::number(currentDateTime.date().day());
    rk += QString::number(currentDateTime.time().hour());
    rk += QString::number(currentDateTime.time().minute());
    rk += QString::number(currentDateTime.time().second());
    this->resv_key = rk;
}

void Reservation::setUserName(QString user_name){
    this->user_name = user_name;
}

void Reservation::setResvType(int resv_type){
    this->resv_type = resv_type;
}

void Reservation::setResvContent(QString resv_content){
    this->resv_content = resv_content;
}

void Reservation::setResvAvail(bool resv_avail){
    this->resv_avail = resv_avail;
}

void Reservation::setResvKey(QString resv_key){
    this->resv_key = resv_key;
}

QString Reservation::getUserName(){
    return this->user_name;
}

int Reservation::getResvType(){
    return this->resv_type;
}

QString Reservation::getResvContent(){
    return this->resv_content;
}

bool Reservation::getResvAvail(){
    return this->resv_avail;
}

QString Reservation::getResvKey(){
    return this->resv_key;
}

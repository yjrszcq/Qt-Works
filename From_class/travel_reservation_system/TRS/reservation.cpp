#include "reservation.h"

Reservation::Reservation(QString user_name){
    this->user_name = user_name;
    this->resv_type = 0;
    this->resv_key = "";
}

Reservation::Reservation(QString user_name, int resv_type, QString resv_key){
    this->user_name = user_name;
    this->resv_type = resv_type;
    this->resv_key = resv_key;
}

void Reservation::setUserName(QString user_name){
    this->user_name = user_name;
}

void Reservation::setResvType(int resv_type){
    this->resv_type = resv_type;
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

QString Reservation::getResvKey(){
    return this->resv_key;
}

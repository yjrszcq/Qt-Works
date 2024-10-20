#include "flight.h"

Flight::Flight(QString flight_num) {
    this->flight_num = flight_num;
    this->price = 0;
    this->num_seats = 0;
    this->num_avail = 0;
    this->from_city = "";
    this->ariv_city = "";
}

Flight::Flight(QString flight_num, int price, int num_seats, int num_avail, QString from_city, QString ariv_city){
    this->flight_num = flight_num;
    this->price = price;
    this->num_seats = num_seats;
    this->num_avail = num_avail;
    this->from_city = from_city;
    this->ariv_city = ariv_city;
}

void Flight::setFlightNum(QString flight_num){
    this->flight_num = flight_num;
}

void Flight::setPrice(int price){
    this->price = price;
}

void Flight::setNumSeats(int num_seats){
    this->num_seats = num_seats;
}

void Flight::setNumAvail(int num_avail){
    this->num_avail = num_avail;
}

void Flight::setFromCity(QString from_city){
    this->flight_num = from_city;
}

void Flight::setArivCity(QString ariv_city){
    this->ariv_city = ariv_city;
}

QString Flight::getFlightNum(){
    return flight_num;
}

int Flight::getPrice(){
    return price;
}

int Flight::getNumSeats(){
    return num_seats;
}

int Flight::getNumAvail(){
    return num_avail;
}

QString Flight::getFromCity(){
    return from_city;
}

QString Flight::getArivCity(){
    return ariv_city;
}

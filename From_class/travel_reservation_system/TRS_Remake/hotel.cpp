#include "hotel.h"

Hotel::Hotel(QString location){
    this->location = location;
    this->price = 0;
    this->num_rooms = 0;
    this->num_avail = 0;
}

Hotel::Hotel(QString location, int price, int num_rooms, int num_avail){
    this->location = location;
    this->price = price;
    this->num_rooms = num_rooms;
    this->num_avail = num_avail;
}

void Hotel::setLocation(QString location){
    this->location = location;
}

void Hotel::setPrice(int price){
    this->price = price;
}

void Hotel::setNumRooms(int num_rooms){
    this->num_rooms = num_rooms;
}

void Hotel::setNumAvail(int num_avail){
    this->num_avail = num_avail;
}

QString Hotel::getLocation(){
    return this->location;
}

int Hotel::getPrice(){
    return this->price;
}

int Hotel::getNumRooms(){
    return this->num_rooms;
}

int Hotel::getNumAvail(){
    return this->num_avail;
}

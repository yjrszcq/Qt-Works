#include "bus.h"

Bus::Bus(QString location){
    this->location = location;
    this->price = 0;
    this->num_bus = 0;
    this->num_avail = 0;
}

Bus::Bus(QString location, int price, int num_bus, int num_avail){
    this->location = location;
    this->price = price;
    this->num_bus = num_bus;
    this->num_avail = num_avail;
}

void Bus::setLocation(QString location){
    this->location = location;
}

void Bus::setPrice(int price){
    this->price = price;
}

void Bus::setNumBus(int num_bus){
    this->num_bus = num_bus;
}

void Bus::setNumAvail(int num_avail){
    this->num_avail = num_avail;
}

QString Bus::getLocation(){
    return this->location;
}

int Bus::getPrice(){
    return this->price;
}

int Bus::getNumBus(){
    return this->num_bus;
}

int Bus::getNumAvail(){
    return this->num_avail;
}

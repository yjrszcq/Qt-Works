#ifndef HOTEL_H
#define HOTEL_H

#include <QString>

class Hotel
{
public:
    Hotel(QString location);
    Hotel(QString location, int price, int num_rooms, int num_avail);
    void setLocation(QString location);
    void setPrice(int price);
    void setNumRooms(int num_rooms);
    void setNumAvail(int num_avail);
    QString getLocation();
    int getPrice();
    int getNumRooms();
    int getNumAvail();

private:
    QString location;
    int price;
    int num_rooms;
    int num_avail;
};

#endif // HOTEL_H

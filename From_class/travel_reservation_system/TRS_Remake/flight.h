#ifndef FLIGHT_H
#define FLIGHT_H

#include <QString>

class Flight
{
public:
    Flight(QString flight_num);
    Flight(QString flight_num, int price, int num_seats, int num_avail, QString from_city, QString ariv_city);
    void setFlightNum(QString flight_num);
    void setPrice(int price);
    void setNumSeats(int num_seats);
    void setNumAvail(int num_avail);
    void setFromCity(QString from_city);
    void setArivCity(QString ariv_city);
    QString getFlightNum();
    int getPrice();
    int getNumSeats();
    int getNumAvail();
    QString getFromCity();
    QString getArivCity();

private:
    QString flight_num;
    int price;
    int num_seats;
    int num_avail;
    QString from_city;
    QString ariv_city;
};

#endif // FLIGHT_H

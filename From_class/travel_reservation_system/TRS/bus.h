#ifndef BUS_H
#define BUS_H

#include <QString>

class Bus{
public:
    Bus(QString location);
    Bus(QString location, int price, int num_bus, int num_avail);
    void setLocation(QString location);
    void setPrice(int price);
    void setNumBus(int num_bus);
    void setNumAvail(int num_avail);
    QString getLocation();
    int getPrice();
    int getNumBus();
    int getNumAvail();

private:
    QString location;
    int price;
    int num_bus;
    int num_avail;
};
#endif // BUS_H

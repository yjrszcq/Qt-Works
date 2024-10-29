#ifndef MYRESVDIALOG_H
#define MYRESVDIALOG_H

#include "flight.h"
#include "hotel.h"
#include "bus.h"

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class MyResvDialog : public QDialog
{
    Q_OBJECT
public:
    MyResvDialog(Flight flight);
    MyResvDialog(Hotel hotel);
    MyResvDialog(Bus bus);
    static void show(Flight flight){
        MyResvDialog mrcd(flight);
        mrcd.exec();
    }
    static void show(Hotel hotel){
        MyResvDialog mrcd(hotel);
        mrcd.exec();
    }
    static void show(Bus bus){
        MyResvDialog mrcd(bus);
        mrcd.exec();
    }

private:
    QPushButton *pb_ok;
    QLabel *l_0, *l_1, *l_2, *l_3;
    QLineEdit *le_0, *le_1, *le_2, *le_3;
};

#endif // MYRESVDIALOG_H

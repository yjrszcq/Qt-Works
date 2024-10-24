#ifndef SERVER_H
#define SERVER_H

#include "mysqldb.h"
#include "user.h"
#include "flight.h"
#include "hotel.h"
#include "bus.h"
#include "reservation.h"

#include <QObject>
#include <bits/stdc++.h>
#include <QString>
#include <QList>

class Server : public QObject
{
    Q_OBJECT

public:
    enum Status{UNAVAILABLE, AVAILABLE};
    Server(QString host = "localhost", int port = 3306, QString name = "root", QString password = "1234", QString database = "travel_reservation");
    bool connectToDatabase(QString host, int port, QString name, QString password, QString database);
    bool login(User user);
    void logout();
    void signup(User user);
    bool reserve(Reservation reservation);
    void addItem(Flight flight);
    void addItem(Hotel hotel);
    void addItem(Bus bus);
    void addItem(User user);
    void addItem(Reservation reservation);
    QList<QHash<QString,QString>> getData(int flag);
    User* getCurrentUser();
    Status getStatus();

private:
    MysqlDb *db = NULL;
    User *currentUser = NULL;
    void delay(int milliseconds);
    bool startServer(QString host, int port, QString name, QString password, QString database);
    Status status;

public slots:
    void dbSetReceive(QString host, int port, QString name, QString password, QString database, bool flag);

signals:
    void exitSent(int flag);
    void userSent(User* currentUser);
    void refreshSent(int flag);
};

#endif // SERVER_H

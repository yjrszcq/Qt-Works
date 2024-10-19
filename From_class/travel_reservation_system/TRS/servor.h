#ifndef SERVOR_H
#define SERVOR_H

#include "mysqldb.h"
#include "user.h"

#include <QObject>
#include <bits/stdc++.h>
#include <QString>
using namespace std;

class Servor : public QObject
{
    Q_OBJECT

public:
    Servor(QString host = "localhost", int port = 3306, QString name = "root", QString password = "1234", QString database = "travel_reservation");
    bool connectToDatabase(QString host, int port, QString name, QString password, QString database);
    bool login(QString name, QString id, int flag);

private:
    MysqlDb *db;
    User *currentUser;
    void delay(int milliseconds);
    bool startServor(QString host, int port, QString name, QString password, QString database);

public slots:
    void dbSetReceive(QString host, int port, QString name, QString password, QString database, bool flag);

signals:
    void exitSent(int flag);
};

#endif // SERVOR_H
#ifndef SERVER_H
#define SERVER_H

#include "mysqldb.h"
#include "user.h"
#include "flight.h"
#include "hotel.h"
#include "bus.h"
#include "reservation.h"

#include <QObject>
#include <QSettings>

class Server : public QObject
{
    Q_OBJECT

public:
    enum Status{UNAVAILABLE, AVAILABLE};
    Server();
    QList<MysqlDb::SqlTable> dbTables();
    bool connectToDatabase();
    bool login(User user);
    void logout();
    void signup(User user);
    bool reserve(Reservation reservation);
    void insertItem(Flight flight);
    void insertItem(Hotel hotel);
    void insertItem(Bus bus);
    void insertItem(User user);
    void insertItem(Reservation reservation);
    void updateItem(Flight flight);
    void updateItem(Hotel hotel);
    void updateItem(Bus bus);
    void updateItem(User user);
    void updateItem(Reservation reservation);
    void deleteItem(int flag, QString content);
    QList<QHash<QString,QString>> getData(int flag);
    QList<QHash<QString,QString>> getData(User user);
    QList<QHash<QString,QString>> getData(int flag,  int attribute_flag, QString search_content);
    QHash<QString,QString> getData(int flag, QString key);
    User* getCurrentUser();
    Status getStatus();

private:
    MysqlDb *db = NULL;
    User *currentUser = NULL;
    const QString config_file_path = "config.ini";
    Status status;
    bool startServer();
    void initConfig();
    bool CheckConfig();
    QSettings readConfig();

public slots:
    void dbSetReceive( bool flag);

signals:
    void userSent(User* currentUser);
    void refreshSent(int flag);
};

#endif // SERVER_H

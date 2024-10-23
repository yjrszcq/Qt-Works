#include "server.h"
#include "setdbdialog.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QRegularExpression>

Server::Server(QString host, int port, QString name, QString password, QString database) {
    status = Server::UNAVAILABLE;
    if(startServer(host, port, name, password, database)){
        status = Server::AVAILABLE;
        // QTimer::singleShot(0, this, [this](){ emit exitSent(1); });
    }
}

bool Server::startServer(QString host, int port, QString name, QString password, QString database){
    connectToDatabase(host, port, name, password, database);
    if(db == NULL){
        QMessageBox *msg = new QMessageBox(QMessageBox::Critical, "错误", "数据库连接出错，是否重新连接数据库？", QMessageBox::Yes | QMessageBox::No, NULL);
        SetDbDialog *sdd = new SetDbDialog(host, port, name, password, database);
        connect(sdd, SIGNAL(dbSetSent(QString, int, QString, QString, QString, bool)), this, SLOT(dbSetReceive(QString, int, QString, QString, QString, bool)));
        while(db == NULL){
            switch(msg->exec()){
            case QMessageBox::Yes: {
                sdd->exec();
                break;
            }
            case QMessageBox::No: {delete msg; delete sdd; return false;}
            default: {delete msg; delete sdd; return false;}
            }
        }
        delete msg;
        delete sdd;
    }
    currentUser = new User(User::VISITOR);
    return true;
}

void Server::dbSetReceive(QString host, int port, QString name, QString password, QString database, bool flag){
    if(flag){
        connectToDatabase(host, port, name, password, database);
    }
}

bool Server::connectToDatabase(QString host, int port, QString name, QString password, QString database){
    db = new MysqlDb();
    db->setMhost(host);
    db->setMport(port);
    db->setMuser(name);
    db->setMpwd(password);
    bool jud = db->connectSql(database);
    if (jud){
        qDebug() << "Succeed";
        return true;
    }
    else {
        qDebug() << "Error open database because" << db->getErrorSql();
        delete db;
        db = NULL;
        return false;
    }
}

bool Server::login(User tempUser){
    if(currentUser != NULL){
        delete currentUser;
    }
    if(tempUser.getPermission() == User::USER){
        QHash<QString, QString> user;
        QString sqlWhere = "where custName = '" + tempUser.getName() + "'";
        bool jud = db->getData("customers", user, sqlWhere);
        if(jud){
            if(tempUser.getName() == user["custName"] && tempUser.getPassword() == user["custPW"]){
                currentUser = new User(user["custName"], user["custID"], user["custPW"], User::USER);
                emit userSent(currentUser);
                return true;
            } else{
                QMessageBox::critical(NULL, "critical", "密码错误", QMessageBox::Yes);
                currentUser = new User(User::VISITOR);
                emit userSent(currentUser);
                return false;
            }
        } else{
            QMessageBox::critical(NULL, "critical", "用户名错误", QMessageBox::Yes);
            currentUser = new User(User::VISITOR);
            emit userSent(currentUser);
            return false;
        }
    } else if(tempUser.getPermission() == User::ROOT){
        if(tempUser.getPassword() == "1234"){
            currentUser = new User(User::ROOT);
            emit userSent(currentUser);
            return true;
        } else{
            QMessageBox::critical(NULL, "critical", "密码错误", QMessageBox::Yes);
            currentUser = new User(User::VISITOR);
            emit userSent(currentUser);
            return false;
        }
    } else{
        QMessageBox::critical(NULL, "critical", "出错啦：Login flag is wrong", QMessageBox::Yes);
        currentUser = new User(User::VISITOR);
        emit userSent(currentUser);
        return false;
    }
}

void Server::logout(){
    QMessageBox::StandardButton result = QMessageBox::warning(NULL, "提示", "是否退出登录？", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes){
        delete currentUser;
        currentUser = new User(User::VISITOR);
        emit userSent(currentUser);
    }
}

void Server::signup(User tempUser){
    if(tempUser.getName() != "" && tempUser.getId() != "" && tempUser.getPassword() != ""){
        QHash<QString, QString> user;
        user.insert("custName", tempUser.getName());
        user.insert("custID", tempUser.getId());
        user.insert("custPW", tempUser.getPassword());
        if(db->addData("customers", user)){
            QMessageBox::information(NULL, "成功", "注册成功", QMessageBox::Yes);
        } else{
            QMessageBox::information(NULL, "失败", "注册失败", QMessageBox::Yes);
        }
    } else{
        QMessageBox::critical(NULL, "错误", "用户名和ID和密码不能为空", QMessageBox::Yes);
    }
}

bool Server::reserve(Reservation tempResv){
    if(tempResv.getUserName() != "" && tempResv.getResvType() >= 0 && tempResv.getResvType() <= 2 && tempResv.getResvContent() != ""){
        QHash<QString, QString> resv;
        resv.insert("custName", tempResv.getUserName());
        resv.insert("resvType", QString::number(tempResv.getResvType()));
        resv.insert("resvContent", tempResv.getResvContent());
        resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
        resv.insert("resvKey", tempResv.getResvKey());
        if(!db->addData("reservations", resv)){
            QMessageBox::information(NULL, "失败", "预订失败-1", QMessageBox::Yes);
            return false;
        }
        QString sql_where_delete = "where resvKey = '" + resv["resvKey"] + "'";
        // 飞机，宾馆，客车的剩余数 -1
        switch(tempResv.getResvType()){
        case Reservation::FLIGHT: {
            QHash<QString, QString> flight;
            // 查询航班信息
            QString sql_where = "where flightNum = '" + tempResv.getResvContent() + "'";
            bool jud_get_data = db->getData("flights", flight, sql_where);
            if(!jud_get_data){
                QMessageBox::critical(NULL, "critical", "该航班不存在, 预订失败-2", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            // 剩余座位 -1
            bool ok;
            int avail = flight["numAvail"].toInt(&ok, 10) - 1;
            if(avail < 0){
                QMessageBox::critical(NULL, "critical", "该航班已满座, 预订失败-3", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            // 更新航班信息
            flight.insert("numAvail", QString::number(avail));
            bool jud_update_data = db->updateData("flights", flight, sql_where);
            if(!jud_update_data){
                QMessageBox::information(NULL, "失败", "预订失败-4", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            break;
        }
        case Reservation::HOTEL:{
            QHash<QString, QString> hotel;
            // 查询宾馆信息
            QString sql_where = "where location = '" + tempResv.getResvContent() + "'";
            bool jud_get_data = db->getData("hotels", hotel, sql_where);
            if(!jud_get_data){
                QMessageBox::critical(NULL, "critical", "该宾馆不存在, 预订失败-2", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            // 剩余房间 -1
            bool ok;
            int avail = hotel["numAvail"].toInt(&ok, 10) - 1;
            if(avail < 0){
                QMessageBox::critical(NULL, "critical", "该宾馆已住满, 预订失败-3", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            // 更新宾馆信息
            hotel.insert("numAvail", QString::number(avail));
            bool jud_update_data = db->updateData("hotels", hotel, sql_where);
            if(!jud_update_data){
                QMessageBox::information(NULL, "失败", "预订失败-4", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            break;
        }
        case Reservation::BUS:{
            QHash<QString, QString> bus;
            // 查询客车信息
            QString sql_where = "where location = '" + tempResv.getResvContent() + "'";
            bool jud_get_data = db->getData("bus", bus, sql_where);
            if(!jud_get_data){
                QMessageBox::critical(NULL, "critical", "该客车不存在, 预订失败-2", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            // 剩余座位 -1
            bool ok;
            int avail = bus["numAvail"].toInt(&ok, 10) - 1;
            if(avail < 0){
                QMessageBox::critical(NULL, "critical", "该客车已满座, 预订失败-3", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            // 更新客车信息
            bus.insert("numAvail", QString::number(avail));
            bool jud_update_data = db->updateData("bus", bus, sql_where);
            if(!jud_update_data){
                QMessageBox::information(NULL, "失败", "预订失败-4", QMessageBox::Yes);
                db->delData("reservations", sql_where_delete);
                return false;
            }
            break;
        }
        }
        QMessageBox::information(NULL, "成功", "预订成功", QMessageBox::Yes);

        // 刷新table_view
        emit refreshSent(tempResv.getResvType());
        return true;
    } else{
        QMessageBox::information(NULL, "失败", "有错误或空属性值", QMessageBox::Yes);
        return false;
    }
}

QList<QHash<QString,QString>> Server::getData(int flag){
    switch(flag){
    case 0:{
        QList<QHash<QString,QString>> data;
        db->getData("flights", data, "");
        return data;
    }
    case 1:{
        QList<QHash<QString,QString>> data;
        db->getData("hotels", data, "");
        return data;
    }
    case 2:{
        QList<QHash<QString,QString>> data;
        db->getData("bus", data, "");
        return data;
    }
    case 3: {
        QList<QHash<QString,QString>> data;
        db->getData("customers", data, "");
        return data;
    }
    case 4:{
        QList<QHash<QString,QString>> data;
        db->getData("reservations", data, "");
        return data;
    }
    default: {
        QList<QHash<QString,QString>> data;
        QMessageBox::critical(NULL, "错误", "未知数据请求", QMessageBox::Yes);
        return data;
    }
    }

}

User* Server::getCurrentUser(){
    return currentUser;
}

Server::Status Server::getStatus(){
    return status;
}

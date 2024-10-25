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
        if(currentUser->getPermission() == User::USER){
            emit refreshSent(tempResv.getResvType());
        } else{
            emit refreshSent(4);
        }
        return true;
    } else{
        QMessageBox::information(NULL, "失败", "有错误或空属性值", QMessageBox::Yes);
        return false;
    }
}

void Server::insertItem(Flight tempFlight){
    if(tempFlight.getFlightNum() != "" && tempFlight.getFromCity() != "" && tempFlight.getArivCity() != ""){
        QHash<QString, QString> flight;
        flight.insert("flightNum", tempFlight.getFlightNum());
        flight.insert("price", QString::number(tempFlight.getPrice()));
        flight.insert("numSeats", QString::number(tempFlight.getNumSeats()));
        flight.insert("numAvail", QString::number(tempFlight.getNumAvail()));
        flight.insert("FromCity", tempFlight.getFromCity());
        flight.insert("ArivCity", tempFlight.getArivCity());
        if(db->addData("flights", flight)){
            QMessageBox::information(NULL, "成功", "添加成功", QMessageBox::Yes);
            emit refreshSent(0);
        } else{
            QMessageBox::information(NULL, "失败", "添加失败-可能是航班号重复", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "添加失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::insertItem(Hotel tempHotel){
    if(tempHotel.getLocation() != ""){
        QHash<QString, QString> hotel;
        hotel.insert("location", tempHotel.getLocation());
        hotel.insert("price", QString::number(tempHotel.getPrice()));
        hotel.insert("numRooms", QString::number(tempHotel.getNumRooms()));
        hotel.insert("numAvail", QString::number(tempHotel.getNumAvail()));
        if(db->addData("hotels", hotel)){
            QMessageBox::information(NULL, "成功", "添加成功", QMessageBox::Yes);
            emit refreshSent(1);
        } else{
            QMessageBox::information(NULL, "失败", "添加失败-可能是所在地重复", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "添加失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::insertItem(Bus tempBus){
    if(tempBus.getLocation() != ""){
        QHash<QString, QString> bus;
        bus.insert("location", tempBus.getLocation());
        bus.insert("price", QString::number(tempBus.getPrice()));
        bus.insert("numBus", QString::number(tempBus.getNumBus()));
        bus.insert("numAvail", QString::number(tempBus.getNumAvail()));
        if(db->addData("bus", bus)){
            QMessageBox::information(NULL, "成功", "添加成功", QMessageBox::Yes);
            emit refreshSent(2);
        } else{
            QMessageBox::information(NULL, "失败", "添加失败-可能是所在地重复", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "添加失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::insertItem(User tempUser){
    if(tempUser.getName() != "" && tempUser.getId() != "" && tempUser.getPassword() != ""){
        QHash<QString, QString> user;
        user.insert("custName", tempUser.getName());
        user.insert("custID", tempUser.getId());
        user.insert("custPW", tempUser.getPassword());
        if(db->addData("customers", user)){
            QMessageBox::information(NULL, "成功", "添加成功", QMessageBox::Yes);
            emit refreshSent(3);
        } else{
            QMessageBox::information(NULL, "失败", "添加失败-可能是用户名重复", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "添加失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::insertItem(Reservation reservation){
    reserve(reservation);
}

void Server::updateItem(Flight tempFlight){
    if(tempFlight.getFlightNum() != "" && tempFlight.getFromCity() != "" && tempFlight.getArivCity() != ""){
        QHash<QString, QString> flight;
        flight.insert("price", QString::number(tempFlight.getPrice()));
        flight.insert("numSeats", QString::number(tempFlight.getNumSeats()));
        flight.insert("numAvail", QString::number(tempFlight.getNumAvail()));
        flight.insert("FromCity", tempFlight.getFromCity());
        flight.insert("ArivCity", tempFlight.getArivCity());
        QString sql_where = "where flightNum = '" + tempFlight.getFlightNum() + "'";
        if(db->updateData("flights", flight, sql_where)){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(0);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是航班不存在或其他格式错误", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::updateItem(Hotel tempHotel){
    if(tempHotel.getLocation() != ""){
        QHash<QString, QString> hotel;
        hotel.insert("price", QString::number(tempHotel.getPrice()));
        hotel.insert("numRooms", QString::number(tempHotel.getNumRooms()));
        hotel.insert("numAvail", QString::number(tempHotel.getNumAvail()));
        QString sql_where = "where location = '" + tempHotel.getLocation() + "'";
        if(db->updateData("hotels", hotel, sql_where)){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(1);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是所在地不存在或其他格式错误", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::updateItem(Bus tempBus){
    if(tempBus.getLocation() != ""){
        QHash<QString, QString> bus;
        bus.insert("price", QString::number(tempBus.getPrice()));
        bus.insert("numBus", QString::number(tempBus.getNumBus()));
        bus.insert("numAvail", QString::number(tempBus.getNumAvail()));
        QString sql_where = "where location = '" + tempBus.getLocation() + "'";
        if(db->updateData("bus", bus, sql_where)){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(2);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是所在地不存在或其他格式错误", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::updateItem(User tempUser){
    if(tempUser.getName() != "" && tempUser.getId() != "" && tempUser.getPassword() != ""){
        QHash<QString, QString> user;
        user.insert("custID", tempUser.getId());
        user.insert("custPW", tempUser.getPassword());
        QString sql_where = "where custName = " + tempUser.getName() + "'";
        if(db->updateData("customers", user, sql_where)){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(3);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是用户名不存在或其他格式错误", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::updateItem(Reservation tempResv){
    if(tempResv.getResvKey() != ""){
        QHash<QString, QString> resv;
        resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
        QString sql_where = "where resvKey = '" + tempResv.getResvKey() + "'";
        if(db->updateData("reservations", resv, sql_where)){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(4);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是预订不存在或其他格式错误", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::deleteItem(int flag, QString content, bool avail){
    if(content != ""){
        int jud = 4;
        switch(flag){
        case 0: {
            if(){ // 询问是否删除（若删除则相关预订不可用）
                QString sql_where = "where flightNum = '" + content + "'";
                if(db->delData("flights", sql_where)){
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 1: {
            if(){
                QString sql_where = "where location = '" + content + "'";
                if(db->delData("hotels", sql_where)){
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 2: {
            if(){
                QString sql_where = "where location = '" + content + "'";
                if(db->delData("bus", sql_where)){
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 3: {
            if(){
                QString sql_where = "where custName = '" + content + "'";
                if(db->delData("customers", sql_where)){
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 4: {
            if(avail == Reservation::AVAILABLE){
                QMessageBox *msg = new QMessageBox(QMessageBox::Warning, "警告",
                                                   "删除可用预订会将预订占用的内容归还，是否继续？",
                                                   QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec() == QMessageBox::Yes){
                    // 归还预订内容
                    QString table;
                    QString sql_where;
                    switch(content[0].unicode()){
                    case 'F': table = "flights"; sql_where = "where flightNum = '" + content + "'"; break;
                    case 'H': table = "hotels"; sql_where = "where location = '" + content + "'"; break;
                    case 'B': table = "bus"; sql_where = "where location = '" + content + "'"; break;
                    }
                    QHash<QString, QString> item;
                    bool ok;
                    bool jud_gd = db->getData(table, item, sql_where);
                    item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) + 1));
                    bool jud_ud = db->updateData(table, item, sql_where);
                    if(jud_gd && jud_ud && ok){
                        QString sql_where_resv = "where resvKey = '" +content + "'";
                        if(db->delData("reservations", sql_where_resv)){
                            jud = 0;
                        } else{
                            jud = 1;
                        }
                    } else{
                        jud = 2;
                    }
                } else{
                    jud = 3;
                }
                delete msg;
            } else{
                QString sql_where = "where resvKey = '" +content + "'";
                if(db->delData("reservations", sql_where)){
                    jud = 0;
                }
            }
            break;
        }
        }
        switch(jud){
        case 0: QMessageBox::information(NULL, "成功", "删除成功", QMessageBox::Yes); emit refreshSent(flag); break;
        case 1: QMessageBox::information(NULL, "失败", "删除失败-可能是主键值不存在", QMessageBox::Yes); break;
        case 2: QMessageBox::information(NULL, "失败", "删除失败-可能是相关的表更改失败", QMessageBox::Yes); break;
        case 3: QMessageBox::information(NULL, "失败", "删除失败-取消删除", QMessageBox::Yes); break;
        case 4: QMessageBox::information(NULL, "失败", "删除失败-其他错误", QMessageBox::Yes); break;
        }
    } else{
        QMessageBox::information(NULL, "失败", "删除失败-主键值不能未空", QMessageBox::Yes);
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

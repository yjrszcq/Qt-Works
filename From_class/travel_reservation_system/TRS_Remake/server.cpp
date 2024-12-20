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
    }
}

bool Server::startServer(QString host, int port, QString name, QString password, QString database){
    connectToDatabase(host, port, name, password, database);
    if(db == NULL){
        QMessageBox *msg = new QMessageBox(
            QMessageBox::Critical,
            "错误", "数据库连接出错，是否重新连接数据库？",
            QMessageBox::Yes | QMessageBox::No, NULL);
        SetDbDialog *sdd = new SetDbDialog(host, port, name, password, database);
        connect(sdd, SIGNAL(dbSetSent(QString, int, QString, QString, QString, bool)),
                this, SLOT(dbSetReceive(QString, int, QString, QString, QString, bool)));
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
        db->getData("customers", user, sqlWhere);
        if(user.size() != 0){
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
            QMessageBox::critical(NULL, "critical", "用户名不存在", QMessageBox::Yes);
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
        db->addData("customers", user);
        if(db->getNumRowsAffected() != 0){
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
        resv.insert("resvNote", tempResv.getResvNote());
        db->addData("reservations", resv);
        if(db->getNumRowsAffected() == 0){
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
            db->getData("flights", flight, sql_where);
            if(flight.size() == 0){
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
            db->updateData("flights", flight, sql_where);
            if(db->getNumRowsAffected() == 0){
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
            db->getData("hotels", hotel, sql_where);
            if(hotel.size() == 0){
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
            db->updateData("hotels", hotel, sql_where);
            if(db->getNumRowsAffected() == 0){
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
            db->getData("bus", bus, sql_where);
            if(bus.size() == 0){
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
            db->updateData("bus", bus, sql_where);
            if(db->getNumRowsAffected() == 0){
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
        QMessageBox::information(NULL, "失败", "有错误 或 空属性值", QMessageBox::Yes);
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
        db->addData("flights", flight);
        if(db->getNumRowsAffected() != 0){
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
        db->addData("hotels", hotel);
        if(db->getNumRowsAffected() != 0){
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
        db->addData("bus", bus);
        if(db->getNumRowsAffected() != 0){
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
        db->addData("customers", user);
        if(db->getNumRowsAffected() != 0){
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
        db->updateData("flights", flight, sql_where);
        if(db->getNumRowsAffected() != 0){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(0);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是 未更改内容 或 航班不存在 或 其他格式错误", QMessageBox::Yes);
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
        db->updateData("hotels", hotel, sql_where);
        if(db->getNumRowsAffected() != 0){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(1);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是 未更改内容 或 所在地不存在 或 其他格式错误", QMessageBox::Yes);
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
        db->updateData("bus", bus, sql_where);
        if(db->getNumRowsAffected() != 0){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(2);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是 未更改内容 或 所在地不存在 或 其他格式错误", QMessageBox::Yes);
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
        db->updateData("customers", user, sql_where);
        if(db->getNumRowsAffected() != 0){
            QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
            emit refreshSent(3);
        } else{
            QMessageBox::information(NULL, "失败", "修改失败-可能是 未更改内容 或 用户名不存在 或 其他格式错误", QMessageBox::Yes);
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::updateItem(Reservation tempResv){
    if(tempResv.getResvKey() != ""){
        QHash<QString, QString> resv_is_change;
        QString sql_where_is_change = "where resvKey = '" + tempResv.getResvKey() + "'";
        db->getData("reservations", resv_is_change, sql_where_is_change);
        if(resv_is_change.size() != 0 && resv_is_change["resvAvail"] != QString::number(tempResv.getResvAvail())){
            QString msg_string;
            if(tempResv.getResvAvail() == Reservation::AVAILABLE){
                msg_string = "将预订可用性更改为'可用'会将预订的内容重新取回，是否继续？";
            } else{
                msg_string = "将预订可用性更改为'不可用'会将预订的内容归还，是否继续？";
            }
            QMessageBox *msg = new QMessageBox(QMessageBox::Warning, "警告", msg_string, QMessageBox::Yes | QMessageBox::No, NULL);
            if(msg->exec() == QMessageBox::Yes){
                QString table;
                QString sql_where;
                switch(tempResv.getResvType()){
                case 0: table = "flights"; sql_where = "where flightNum = '" + tempResv.getResvContent() + "'"; break;
                case 1: table = "hotels"; sql_where = "where location = '" + tempResv.getResvContent() + "'"; break;
                case 2: table = "bus"; sql_where = "where location = '" + tempResv.getResvContent() + "'"; break;
                }
                QHash<QString, QString> item;
                bool ok;
                db->getData(table, item, sql_where);
                if(item.size() == 0){
                    if(resv_is_change["resvAvail"] == QString::number(Reservation::UNAVAILABLE)){
                        QMessageBox::information(NULL, "失败", "修改失败-无法归还或取回内容-内容不存在，可能已被删除", QMessageBox::Yes);
                        return;
                    } else{
                        QHash<QString, QString> resv;
                        resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
                        QString sql_where_resv = "where resvKey = '" + tempResv.getResvKey() + "'";
                        db->updateData("reservations", resv, sql_where_resv);
                        QMessageBox::information(NULL, "成功", "修改成功-但原先可用的内容并不存在，请检查数据的一致性", QMessageBox::Yes);
                        emit refreshSent(4);
                        return;
                    }

                }
                if(tempResv.getResvAvail()){
                    int current_avail = item["numAvail"].toInt(&ok, 10);
                    if(current_avail > 0){
                        item.insert("numAvail", QString::number(current_avail - 1));
                    } else{
                        QMessageBox::information(NULL, "失败", "修改失败-无法取回内容-剩余内容已空", QMessageBox::Yes);
                        return;
                    }
                } else{
                    item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) + 1));
                }
                db->updateData(table, item, sql_where);
                bool jud_ud = (db->getNumRowsAffected() != 0 ? true : false);
                if(jud_ud && ok){
                    QHash<QString, QString> resv;
                    resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
                    resv.insert("resvNote", tempResv.getResvNote());
                    QString sql_where_resv = "where resvKey = '" + tempResv.getResvKey() + "'";
                    db->updateData("reservations", resv, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
                        emit refreshSent(4);
                    } else{
                        QMessageBox::information(NULL, "失败", "修改失败-可能是 预订不存在 或 其他格式错误", QMessageBox::Yes);
                    }
                } else{
                    if(jud_ud){
                        if(tempResv.getResvAvail()){
                            item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) + 1));
                        } else{
                            item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) - 1));
                        }
                        db->updateData(table, item, sql_where);
                        QMessageBox::information(NULL, "失败", "修改失败-可能是主键值不存在", QMessageBox::Yes);
                    }
                }
            }
            delete msg;
        } else{
            if(resv_is_change.size() != 0){
                QHash<QString, QString> resv;
                resv.insert("resvNote", tempResv.getResvNote());
                QString sql_where_resv = "where resvKey = '" + tempResv.getResvKey() + "'";
                db->updateData("reservations", resv, sql_where_resv);
                if(db->getNumRowsAffected() != 0){
                    QMessageBox::information(NULL, "成功", "修改成功", QMessageBox::Yes);
                    emit refreshSent(4);
                } else{
                    QMessageBox::information(NULL, "失败", "修改失败-可能是 未更改内容 或 预订不存在 或 其他格式错误", QMessageBox::Yes);
                }
            } else{
                QMessageBox::information(NULL, "失败", "修改失败-其他错误", QMessageBox::Yes);
            }
        }
    } else{
        QMessageBox::information(NULL, "失败", "修改失败-属性不能有空值", QMessageBox::Yes);
    }
}

void Server::deleteItem(int flag, QString content){
    if(content != ""){
        int jud = 3;
        switch(flag){
        case 0: {
            QString sql_where = "where flightNum = '" + content + "'";
            QHash<QString, QString> flight;
            db->getData("flights", flight, sql_where);
            if(flight.size() != 0 && flight["numSeats"] != flight["numAvail"]){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec()){// 询问是否删除（若删除则相关预订不可用）
                    QHash<QString, QString> update;
                    QString sql_where_resv = "where resvType = " + QString::number(flag) + " and resvContent = '" + content + "'";
                    update.insert("resvAvail", "0");
                    db->updateData("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        db->delData("flights", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "内容已被删除");
                            db->updateData("reservations", update, sql_where_resv);
                            emit refreshSent(0);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateData("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    } else{
                        jud = 2;
                    }
                }
                delete msg;
            } else{
                QString sql_where = "where flightNum = '" + content + "'";
                db->delData("flights", sql_where);
                if(db->getNumRowsAffected() != 0){
                    emit refreshSent(0);
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 1: {
            QString sql_where = "where location = '" + content + "'";
            QHash<QString, QString> hotel;
            db->getData("hotels", hotel, sql_where);
            if(hotel.size() != 0 && hotel["numRooms"] != hotel["numAvail"]){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec()){
                    QString sql_where_resv = "where resvType = " + QString::number(flag) + " and resvContent = '" + content + "'";
                    QHash<QString, QString> update;
                    update.insert("resvAvail", "0");
                    db->updateData("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        db->delData("hotels", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "内容已被删除");
                            db->updateData("reservations", update, sql_where_resv);
                            emit refreshSent(1);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateData("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    } else{
                        jud = 2;
                    }
                }
                delete msg;
            } else{
                db->delData("hotels", sql_where);
                if(db->getNumRowsAffected() != 0){
                    emit refreshSent(1);
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 2: {
            QString sql_where = "where location = '" + content + "'";
            QHash<QString, QString> bus;
            db->getData("bus", bus, sql_where);
            if(bus.size() != 0 && bus["numBus"] != bus["numAvail"]){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec()){
                    QString sql_where_resv = "where resvType = " + QString::number(flag) + " and resvContent = '" + content + "'";
                    QHash<QString, QString> update;
                    update.insert("resvAvail", "0");
                    db->updateData("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        db->delData("bus", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "内容已被删除");
                            db->updateData("reservations", update, sql_where_resv);
                            emit refreshSent(2);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateData("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    }
                }
                delete msg;
            } else{
                db->delData("bus", sql_where);
                if(db->getNumRowsAffected() != 0){
                    emit refreshSent(2);
                    jud = 0;
                } else{
                    jud = 1;
                }
            }

            break;
        }
        case 3: {
            QString sql_where_resv = "where resvType = " + QString::number(flag) + " and resvContent = '" + content + "'";
            QList<QHash<QString, QString>> resv;
            db->getData("reservations", resv, sql_where_resv);
            bool jud_ra = false;
            for(auto r : resv){
                if(r["resvAvail"] == "1"){
                    jud_ra = true;
                    break;
                }
            }
            if(jud_ra){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec()){
                    QHash<QString, QString> update;
                    update.insert("resvAvail", "0");
                    db->updateData("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        QString sql_where = "where custName = '" + content + "'";
                        db->delData("customers", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "用户已被删除");
                            db->updateData("reservations", update, sql_where_resv);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateData("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    }
                }
                delete msg;
            } else{
                QString sql_where = "where custName = '" + content + "'";
                db->delData("customers", sql_where);
                if(db->getNumRowsAffected() != 0){
                    jud = 0;
                } else{
                    jud = 1;
                }
            }
            break;
        }
        case 4: {
            QHash<QString, QString> resv;
            QString sql_where_resv = "where resvKey = '" + content + "'";
            db->getData("reservations", resv, sql_where_resv);
            if(resv.size() != 0 && resv["resvAvail"] == QString::number(Reservation::AVAILABLE)){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
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
                    db->getData(table, item, sql_where);
                    if(item.size() != 0){
                        item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) + 1));
                        db->updateData(table, item, sql_where);
                        if(db->getNumRowsAffected() != 0 && ok){
                            db->delData("reservations", sql_where_resv);
                            if(db->getNumRowsAffected() != 0){
                                emit refreshSent(4);
                                jud = 0;
                            } else{
                                item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) - 1));
                                db->updateData(table, item, sql_where);
                                jud = 1;
                            }
                        } else{
                            jud = 2;
                        }
                    }
                }
                delete msg;
            } else{
                if(resv.size() != 0){
                    QString sql_where = "where resvKey = '" +content + "'";
                    db->delData("reservations", sql_where);
                    if(db->getNumRowsAffected() != 0){
                        emit refreshSent(4);
                        jud = 0;
                    } else{
                        jud = 1;
                    }
                }
            }
            break;
        }
        }
        switch(jud){
        case 0: QMessageBox::information(NULL, "成功", "删除成功", QMessageBox::Yes); emit refreshSent(flag); break;
        case 1: QMessageBox::information(NULL, "失败", "删除失败-可能是 主键值不存在 或 内容已被删除", QMessageBox::Yes); break;
        case 2: QMessageBox::information(NULL, "失败", "删除失败-可能是相关的表更改失败", QMessageBox::Yes); break;
        case 3: QMessageBox::information(NULL, "失败", "删除失败-其他错误", QMessageBox::Yes); break;
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

QList<QHash<QString,QString>> Server::getData(User user){
    QList<QHash<QString,QString>> data;
    QString sql_where = "where custName = '" + user.getName() + "'";
    db->getData("reservations", data, sql_where);
    return data;
}

QList<QHash<QString,QString>> Server::getData(int table_flag, int attribute_flag, QString search_content){
    QString table;
    QString attribute;
    QString resv_type;
    QString sql_where;
    switch(table_flag){
    case 0: {
        table = "flights";
        switch(attribute_flag){
        case 0: attribute = "flightNum"; break;
        case 1: attribute = "FromCity"; break;
        case 2: attribute = "ArivCity"; break;
        }
        sql_where = "where " + attribute + " LIKE '%" + search_content + "%'";
        break;
    }
    case 1: {
        table = "hotels";
        switch(attribute_flag){
        case 0: attribute = "location"; break;
        }
        sql_where = "where " + attribute + " LIKE '%" + search_content + "%'";
        break;
    }
    case 2: {
        table = "bus";
        switch(attribute_flag){
        case 0: attribute = "location"; break;
        }
        sql_where = "where " + attribute + " LIKE '%" + search_content + "%'";
        break;
    }
    case 3: {
        table = "customers";
        switch(attribute_flag){
        case 0: attribute = "custName"; break;
        }
        sql_where = "where " + attribute + " LIKE '%" + search_content + "%'";
        break;
    }
    case 4: {
        table = "reservations";
        switch(attribute_flag){
        case 0: attribute = "resvKey"; break;
        case 1: attribute = "custName"; break;
        case 2: attribute = "resvContent"; resv_type = "0"; break;
        case 3: attribute = "resvContent"; resv_type = "1"; break;
        case 4: attribute = "resvContent"; resv_type = "2"; break;
        }
        if(attribute_flag < 2){
            sql_where = "where " + attribute + " LIKE '%" + search_content + "%'";
        } else{
            sql_where = "where resvType = " + resv_type + " and " + attribute + " LIKE '%" + search_content + "%'";
        }
        break;
    }
    }
    QList<QHash<QString,QString>> data;
    db->getData(table, data, sql_where);
    return data;
}

QHash<QString,QString> Server::getData(int flag, QString key){
    QString table;
    QString sql_where;
    switch(flag){
    case 0: table = "flights"; sql_where = "where flightNum = '" + key + "'"; break;
    case 1: table = "hotels"; sql_where = "where location = '" + key + "'"; break;
    case 2: table = "bus"; sql_where = "where location = '" + key + "'"; break;
    case 3: table = "customers"; sql_where = "where custName = '" + key + "'"; break;
    case 4: table = "reservations"; sql_where = "where resvKey = '" + key + "'"; break;
    }
    QHash<QString,QString> data;
    db->getData(table, data, sql_where);
    return data;
}

User* Server::getCurrentUser(){
    return currentUser;
}

Server::Status Server::getStatus(){
    return status;
}

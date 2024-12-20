#include "server.h"

#include "setdbdialog.h"

#include <QMessageBox>
#include <QFile>

Server::Server() {
    status = Server::UNAVAILABLE;
    initConfig();
    if(startServer()){
        status = Server::AVAILABLE;
    }
}

bool Server::startServer(){
    connectToDatabase();
    if(db == NULL){
        QMessageBox *msg = new QMessageBox(
            QMessageBox::Critical,
            "错误", "数据库连接出错，是否重新连接数据库？",
            QMessageBox::Yes | QMessageBox::No, NULL);
        SetDbDialog *sdd = new SetDbDialog(config_file_path);
        connect(sdd, SIGNAL(dbSetSent(bool)), this, SLOT(dbSetReceive(bool)));
        while(db == NULL){
            switch(msg->exec()){
            case QMessageBox::Yes: sdd->exec(); break;
            case QMessageBox::No: delete msg; delete sdd; return false;
            default: delete msg; delete sdd; return false;
            }
        }
        delete msg;
        delete sdd;
    }
    QSettings config = readConfig();
    if(config.value("Database/init").toBool()){
        QString err = db->autoCreateTable(dbTables());
        if(err != ""){
            QMessageBox::critical(nullptr, "错误", "自动建表出错: " + err, QMessageBox::Yes);
            return false;
        }
    }
    currentUser = new User(User::VISITOR);
    return true;
}

QList<MysqlDb::SqlTable> Server::dbTables(){
    QList<MysqlDb::SqlTable> tables;
    MysqlDb::SqlTable table;
    table.table_name = "customers";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS customers (
          custName varchar(30) NOT NULL COMMENT '客户姓名',
          custID varchar(30) DEFAULT NULL COMMENT '客户ID',
          custPW varchar(20) DEFAULT NULL COMMENT '密码',
          PRIMARY KEY (custName)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='客户信息';
    )";
    tables.append(table);
    table.table_name = "flights";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS flights (
          flightNum char(6) NOT NULL COMMENT '航班号',
          price int DEFAULT NULL COMMENT '机票价格',
          numSeats int DEFAULT NULL COMMENT '座位总数',
          numAvail int DEFAULT NULL COMMENT '剩余可订座位数',
          FromCity varchar(100) DEFAULT NULL COMMENT '出发地',
          ArivCity varchar(100) DEFAULT NULL COMMENT '到达地',
          PRIMARY KEY (flightNum)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='航班信息';
    )";
    tables.append(table);
    table.table_name = "hotels";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS hotels (
          location varchar(100) NOT NULL COMMENT '宾馆地址',
          price int DEFAULT NULL COMMENT '入住价格',
          numRooms int DEFAULT NULL COMMENT '房间总数',
          numAvail int DEFAULT NULL COMMENT '剩余可订房间数',
          PRIMARY KEY (location)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='宾馆信息';
    )";
    tables.append(table);
    table.table_name = "bus";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS bus (
          location varchar(100) NOT NULL COMMENT '客车所属地',
          price int DEFAULT NULL COMMENT '客车价格',
          numBus int DEFAULT NULL COMMENT '客车总数',
          numAvail int DEFAULT NULL COMMENT '剩余可订客车数',
          PRIMARY KEY (location)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='客车信息';
    )";
    tables.append(table);
    table.table_name = "reservations";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS reservations (
          resvKey CHAR(30) NOT NULL COMMENT '预订号',
          custName VARCHAR(30) DEFAULT NULL COMMENT '客户姓名',
          resvType INT DEFAULT NULL COMMENT '预订类型 (1: 航班, 2: 宾馆, 3: 客车)',
          resvAvail TINYINT(1) DEFAULT NULL COMMENT '预订可用性',
          resvNote VARCHAR(100) DEFAULT NULL COMMENT '订单备注',
          PRIMARY KEY (resvKey),
          KEY custName (custName),
          CONSTRAINT reservations_ibfk_1 FOREIGN KEY (custName) REFERENCES customers (custName)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='预订信息';
    )";
    tables.append(table);
    table.table_name = "flight_reservations";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS flight_reservations (
          resvKey CHAR(30) NOT NULL,
          flightNum CHAR(6) NOT NULL,
          PRIMARY KEY (resvKey),
          FOREIGN KEY (resvKey) REFERENCES reservations (resvKey),
          FOREIGN KEY (flightNum) REFERENCES flights (flightNum)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='航班预订信息';
    )";
    tables.append(table);
    table.table_name = "hotel_reservations";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS hotel_reservations (
          resvKey CHAR(30) NOT NULL,
          location VARCHAR(100) NOT NULL,
          PRIMARY KEY (resvKey),
          FOREIGN KEY (resvKey) REFERENCES reservations (resvKey),
          FOREIGN KEY (location) REFERENCES hotels (location)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='宾馆预订信息';
    )";
    tables.append(table);
    table.table_name = "bus_reservations";
    table.table_sql = R"(
        CREATE TABLE IF NOT EXISTS bus_reservations (
          resvKey CHAR(30) NOT NULL,
          location VARCHAR(100) NOT NULL,
          PRIMARY KEY (resvKey),
          FOREIGN KEY (resvKey) REFERENCES reservations (resvKey),
          FOREIGN KEY (location) REFERENCES bus (location)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='客车预订信息';
    )";
    tables.append(table);
    return tables;
}

void Server::dbSetReceive(bool flag){
    if(flag){
        connectToDatabase();
    }
}

bool Server::connectToDatabase(){
    QSettings config = readConfig();
    db = new MysqlDb();
    db->setMhost(config.value("Database/host").toString());
    db->setMport(config.value("Database/port").toInt());
    db->setMuser(config.value("Database/user").toString());
    db->setMpwd(config.value("Database/password").toString());
    bool jud = db->connectSql(config.value("Database/database").toString());
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

void Server::initConfig(){
    QFile file(config_file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QSettings *config = new QSettings(config_file_path, QSettings::IniFormat);
        config->setValue("Database/host", "localhost");
        config->setValue("Database/port", 3306);
        config->setValue("Database/user", "root");
        config->setValue("Database/password", "1234");
        config->setValue("Database/database", "travel_reservation");
        config->setValue("Database/init", true);
        config->setValue("Settings/root_password", "1234");
        delete config;
    }
}

bool Server::CheckConfig(){
    QSettings config(config_file_path, QSettings::IniFormat);
    if(config.value("Database/host").isNull()){
        return false;
    }
    if(config.value("Database/port").isNull()){
        return false;
    }
    if(config.value("Database/user").isNull()){
        return false;
    }
    if(config.value("Database/password").isNull()){
        return false;
    }
    if(config.value("Database/database").isNull()){
        return false;
    }
    if(config.value("Database/init").isNull()){
        return false;
    }
    if(config.value("Settings/root_password").isNull()){
        return false;
    }
    return true;
}

QSettings Server::readConfig(){
    if(!CheckConfig()){
        QFile file(config_file_path);
        file.remove();
        initConfig();
    }
    return QSettings(config_file_path, QSettings::IniFormat);
}

bool Server::login(User tempUser){
    if(currentUser != NULL){
        delete currentUser;
    }
    if(tempUser.getPermission() == User::USER){
        QHash<QString, QString> user;
        MysqlDb::SqlWhere sql_where;
        sql_where.str = "where custName = ?";
        sql_where.values.append(tempUser.getName());
        db->getDataPS("customers", user, sql_where);
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
        QSettings config = readConfig();
        QString root_password = config.value("Settings/root_password").toString();
        if(tempUser.getPassword() == root_password){
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
        db->addDataPS("customers", user);
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
        resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
        resv.insert("resvKey", tempResv.getResvKey());
        resv.insert("resvNote", tempResv.getResvNote());
        db->addDataPS("reservations", resv);
        QHash<QString, QString> resvContent;
        resvContent.insert("resvKey", tempResv.getResvKey());
        switch(tempResv.getResvType()){
        case Reservation::FLIGHT: {
            resvContent.insert("flightNum", tempResv.getResvContent());
            db->addDataPS("flight_reservations", resvContent);
            break;
        }
        case Reservation::HOTEL: {
            resvContent.insert("location", tempResv.getResvContent());
            db->addDataPS("hotel_reservations", resvContent);
            break;
        }
        case Reservation::BUS: {
            resvContent.insert("location", tempResv.getResvContent());
            db->addDataPS("bus_reservations", resvContent);
            break;
        }
        }
        if(db->getNumRowsAffected() == 0){
            QMessageBox::information(NULL, "失败", "预订失败-1", QMessageBox::Yes);
            return false;
        }
        MysqlDb::SqlWhere sql_where_delete;
        sql_where_delete.str = "where resvKey = ?";
        sql_where_delete.values.append(resv["resvKey"]);
        // 飞机，宾馆，客车的剩余数 -1
        switch(tempResv.getResvType()){
        case Reservation::FLIGHT: {
            QHash<QString, QString> flight;
            // 查询航班信息
            MysqlDb::SqlWhere sql_where;
            sql_where.str = "where flightNum = ?";
            sql_where.values.append(tempResv.getResvContent());
            db->getDataPS("flights", flight, sql_where);
            if(flight.size() == 0){
                QMessageBox::critical(NULL, "critical", "该航班不存在, 预订失败-2", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("flight_reservations", sql_where_delete);
                return false;
            }
            // 剩余座位 -1
            bool ok;
            int avail = flight["numAvail"].toInt(&ok, 10) - 1;
            if(avail < 0){
                QMessageBox::critical(NULL, "critical", "该航班已满座, 预订失败-3", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("flight_reservations", sql_where_delete);
                return false;
            }
            // 更新航班信息
            flight.insert("numAvail", QString::number(avail));
            db->updateDataPS("flights", flight, sql_where);
            if(db->getNumRowsAffected() == 0){
                QMessageBox::information(NULL, "失败", "预订失败-4", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("flight_reservations", sql_where_delete);
                return false;
            }
            break;
        }
        case Reservation::HOTEL:{
            QHash<QString, QString> hotel;
            // 查询宾馆信息
            MysqlDb::SqlWhere sql_where;
            sql_where.str = "where location = ?";
            sql_where.values.append(tempResv.getResvContent());
            db->getDataPS("hotels", hotel, sql_where);
            if(hotel.size() == 0){
                QMessageBox::critical(NULL, "critical", "该宾馆不存在, 预订失败-2", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("hotel_reservations", sql_where_delete);
                return false;
            }
            // 剩余房间 -1
            bool ok;
            int avail = hotel["numAvail"].toInt(&ok, 10) - 1;
            if(avail < 0){
                QMessageBox::critical(NULL, "critical", "该宾馆已住满, 预订失败-3", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("hotel_reservations", sql_where_delete);
                return false;
            }
            // 更新宾馆信息
            hotel.insert("numAvail", QString::number(avail));
            db->updateDataPS("hotels", hotel, sql_where);
            if(db->getNumRowsAffected() == 0){
                QMessageBox::information(NULL, "失败", "预订失败-4", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("hotel_reservations", sql_where_delete);
                return false;
            }
            break;
        }
        case Reservation::BUS:{
            QHash<QString, QString> bus;
            // 查询客车信息
            MysqlDb::SqlWhere sql_where;
            sql_where.str = "where location = ?";
            sql_where.values.append(tempResv.getResvContent());
            db->getDataPS("bus", bus, sql_where);
            if(bus.size() == 0){
                QMessageBox::critical(NULL, "critical", "该客车不存在, 预订失败-2", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("bus_reservations", sql_where_delete);
                return false;
            }
            // 剩余座位 -1
            bool ok;
            int avail = bus["numAvail"].toInt(&ok, 10) - 1;
            if(avail < 0){
                QMessageBox::critical(NULL, "critical", "该客车已满座, 预订失败-3", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("bus_reservations", sql_where_delete);
                return false;
            }
            // 更新客车信息
            bus.insert("numAvail", QString::number(avail));
            db->updateDataPS("bus", bus, sql_where);
            if(db->getNumRowsAffected() == 0){
                QMessageBox::information(NULL, "失败", "预订失败-4", QMessageBox::Yes);
                db->delDataPS("reservations", sql_where_delete);
                db->delDataPS("bus_reservations", sql_where_delete);
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
        db->addDataPS("flights", flight);
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
        db->addDataPS("hotels", hotel);
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
        db->addDataPS("bus", bus);
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
        db->addDataPS("customers", user);
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
        MysqlDb::SqlWhere sql_where;
        sql_where.str = "where flightNum = ?";
        sql_where.values.append(tempFlight.getFlightNum());
        db->updateDataPS("flights", flight, sql_where);
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
        MysqlDb::SqlWhere sql_where;
        sql_where.str = "where location = ?";
        sql_where.values.append(tempHotel.getLocation());
        db->updateDataPS("hotels", hotel, sql_where);
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
        MysqlDb::SqlWhere sql_where;
        sql_where.str = "where location = ?";
        sql_where.values.append(tempBus.getLocation());
        db->updateDataPS("bus", bus, sql_where);
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
        MysqlDb::SqlWhere sql_where;
        sql_where.str = "where custName = ?";
        sql_where.values.append(tempUser.getName());
        db->updateDataPS("customers", user, sql_where);
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
        MysqlDb::SqlWhere sql_where_is_change;
        sql_where_is_change.str = "where resvKey = ?";
        sql_where_is_change.values.append(tempResv.getResvKey());
        db->getDataPS("reservations", resv_is_change, sql_where_is_change);
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
                MysqlDb::SqlWhere sql_where;
                switch(tempResv.getResvType()){
                case 0: table = "flights"; sql_where.str = "where flightNum = ?"; sql_where.values.append(tempResv.getResvContent()); break;
                case 1: table = "hotels"; sql_where.str = "where location = ?"; sql_where.values.append(tempResv.getResvContent()); break;
                case 2: table = "bus"; sql_where.str = "where location = ?"; sql_where.values.append(tempResv.getResvContent()); break;
                }
                QHash<QString, QString> item;
                bool ok;
                db->getDataPS(table, item, sql_where);
                if(item.size() == 0){
                    if(resv_is_change["resvAvail"] == QString::number(Reservation::UNAVAILABLE)){
                        QMessageBox::information(NULL, "失败", "修改失败-无法归还或取回内容-内容不存在，可能已被删除", QMessageBox::Yes);
                        return;
                    } else{
                        QHash<QString, QString> resv;
                        resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
                        MysqlDb::SqlWhere sql_where_resv;
                        sql_where_resv.str = "where resvKey = ?";
                        sql_where_resv.values.append(tempResv.getResvKey());
                        db->updateDataPS("reservations", resv, sql_where_resv);
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
                db->updateDataPS(table, item, sql_where);
                bool jud_ud = (db->getNumRowsAffected() != 0 ? true : false);
                if(jud_ud && ok){
                    QHash<QString, QString> resv;
                    resv.insert("resvAvail", QString::number(tempResv.getResvAvail()));
                    resv.insert("resvNote", tempResv.getResvNote());
                    MysqlDb::SqlWhere sql_where_resv;
                    sql_where_resv.str = "where resvKey = ?";
                    sql_where_resv.values.append(tempResv.getResvKey());
                    db->updateDataPS("reservations", resv, sql_where_resv);
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
                        db->updateDataPS(table, item, sql_where);
                        QMessageBox::information(NULL, "失败", "修改失败-可能是主键值不存在", QMessageBox::Yes);
                    }
                }
            }
            delete msg;
        } else{
            if(resv_is_change.size() != 0){
                QHash<QString, QString> resv;
                resv.insert("resvNote", tempResv.getResvNote());
                MysqlDb::SqlWhere sql_where_resv;
                sql_where_resv.str = "where resvKey = ?";
                sql_where_resv.values.append(tempResv.getResvKey());
                db->updateDataPS("reservations", resv, sql_where_resv);
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
        int jud = 4;
        switch(flag){
        case 0: {
            MysqlDb::SqlWhere sql_where;
            sql_where.str = "where flightNum = ?";
            sql_where.values.append(content);
            QHash<QString, QString> flight;
            db->getDataPS("flights", flight, sql_where);
            if(flight.size() != 0 && flight["numSeats"] != flight["numAvail"]){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec() == QMessageBox::Yes){// 询问是否删除（若删除则相关预订不可用）
                    QHash<QString, QString> resv_f;
                    MysqlDb::SqlWhere sql_where_resv_f;
                    sql_where_resv_f.str = "where flightNum = ?";
                    sql_where_resv_f.values.append(content);
                    db->getDataPS("flight_reservations", resv_f, sql_where_resv_f);
                    QHash<QString, QString> update;
                    MysqlDb::SqlWhere sql_where_resv;
                    sql_where_resv.str = "where resvKey = ?";
                    sql_where_resv.values.append(resv_f["resvKey"]);
                    update.insert("resvAvail", "0");
                    db->updateDataPS("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        db->delDataPS("flight_reservations", sql_where_resv_f);
                        db->delDataPS("flights", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "内容已被删除");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            emit refreshSent(0);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    } else{
                        jud = 2;
                    }
                }
                delete msg;
            } else{
                MysqlDb::SqlWhere sql_where;
                sql_where.str = "where flightNum = ?";
                sql_where.values.append(content);
                db->delDataPS("flights", sql_where);
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
            MysqlDb::SqlWhere sql_where;
            sql_where.str = "where location = ?";
            sql_where.values.append(content);
            QHash<QString, QString> hotel;
            db->getDataPS("hotels", hotel, sql_where);
            if(hotel.size() != 0 && hotel["numRooms"] != hotel["numAvail"]){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec() == QMessageBox::Yes){
                    QHash<QString, QString> resv_h;
                    MysqlDb::SqlWhere sql_where_resv_h;
                    sql_where_resv_h.str = "where location = ?";
                    sql_where_resv_h.values.append(content);
                    db->getDataPS("hotel_reservations", resv_h, sql_where_resv_h);
                    QHash<QString, QString> update;
                    MysqlDb::SqlWhere sql_where_resv;
                    sql_where_resv.str = "where resvKey = ?";
                    sql_where_resv.values.append(resv_h["resvKey"]);
                    update.insert("resvAvail", "0");
                    db->updateDataPS("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        db->delDataPS("hotel_reservations", sql_where_resv_h);
                        db->delDataPS("hotels", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "内容已被删除");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            emit refreshSent(1);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    } else{
                        jud = 2;
                    }
                }
                delete msg;
            } else{
                db->delDataPS("hotels", sql_where);
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
            MysqlDb::SqlWhere sql_where;
            sql_where.str = "where location = ?";
            sql_where.values.append(content);
            QHash<QString, QString> bus;
            db->getDataPS("bus", bus, sql_where);
            if(bus.size() != 0 && bus["numBus"] != bus["numAvail"]){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除会将相关预订变为不可用预订，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec() == QMessageBox::Yes){
                    QHash<QString, QString> resv_b;
                    MysqlDb::SqlWhere sql_where_resv_b;
                    sql_where_resv_b.str = "where location = ?";
                    sql_where_resv_b.values.append(content);
                    db->getDataPS("bus_reservations", resv_b, sql_where_resv_b);
                    QHash<QString, QString> update;
                    MysqlDb::SqlWhere sql_where_resv;
                    sql_where_resv.str = "where resvKey = ?";
                    sql_where_resv.values.append(resv_b["resvKey"]);
                    update.insert("resvAvail", "0");
                    db->updateDataPS("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        db->delDataPS("bus_reservations", sql_where_resv_b);
                        db->delDataPS("bus", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "内容已被删除");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            emit refreshSent(2);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    }
                }
                delete msg;
            } else{
                db->delDataPS("bus", sql_where);
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
            MysqlDb::SqlWhere sql_where_resv;
            sql_where_resv.str = "where resvType = ? and resvContent = ?";
            sql_where_resv.values.append(QString::number(flag));
            sql_where_resv.values.append(content);
            QList<QHash<QString, QString>> resv;
            db->getDataPS("reservations", resv, sql_where_resv);
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
                if(msg->exec() == QMessageBox::Yes){
                    QHash<QString, QString> update;
                    update.insert("resvAvail", "0");
                    db->updateDataPS("reservations", update, sql_where_resv);
                    if(db->getNumRowsAffected() != 0){
                        MysqlDb::SqlWhere sql_where;
                        sql_where.str = "where custName = ?";
                        sql_where.values.append(content);
                        db->delDataPS("customers", sql_where);
                        if(db->getNumRowsAffected() != 0){
                            update.clear();
                            update.insert("resvNote", "用户已被删除");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            jud = 0;
                        } else{
                            update.insert("resvAvail", "1");
                            db->updateDataPS("reservations", update, sql_where_resv);
                            jud = 1;
                        }
                    }
                }
                delete msg;
            } else{
                MysqlDb::SqlWhere sql_where;
                sql_where.str = "where custName = ?";
                sql_where.values.append(content);
                db->delDataPS("customers", sql_where);
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
            MysqlDb::SqlWhere sql_where_resv;
            sql_where_resv.str = "where resvKey = ?";
            sql_where_resv.values.append(content);
            db->getDataPS("reservations", resv, sql_where_resv);
            if(resv.size() != 0 && resv["resvAvail"] == QString::number(Reservation::AVAILABLE)){
                QMessageBox *msg = new QMessageBox(
                    QMessageBox::Warning, "警告",
                    "删除可用预订会将预订占用的内容归还，是否继续？",
                    QMessageBox::Yes | QMessageBox::No, NULL);
                if(msg->exec() == QMessageBox::Yes){
                    // 归还预订内容
                    QString table;
                    MysqlDb::SqlWhere sql_where;
                    switch(content[0].unicode()){
                    case 'F': table = "flights"; sql_where.str = "where flightNum = ?"; break;
                    case 'H': table = "hotels"; sql_where.str = "where location = ?"; break;
                    case 'B': table = "bus"; sql_where.str = "where location = ?"; break;
                    }
                    sql_where.values.append(content);
                    QHash<QString, QString> item;
                    bool ok;
                    db->getDataPS(table, item, sql_where);
                    if(item.size() != 0){
                        item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) + 1));
                        db->updateDataPS(table, item, sql_where);
                        if(db->getNumRowsAffected() != 0 && ok){
                            db->delDataPS("reservations", sql_where_resv);
                            switch(content[0].unicode()){
                            case 'F': db->delDataPS("flight_reservations", sql_where_resv); break;
                            case 'H': db->delDataPS("hotel_reservations", sql_where_resv); break;
                            case 'B': db->delDataPS("bus_reservations", sql_where_resv); break;
                            }
                            if(db->getNumRowsAffected() != 0){
                                emit refreshSent(4);
                                jud = 0;
                            } else{
                                item.insert("numAvail", QString::number(item["numAvail"].toInt(&ok, 10) - 1));
                                db->updateDataPS(table, item, sql_where);
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
                    MysqlDb::SqlWhere sql_where;
                    sql_where.str = "where resvKey = ?";
                    sql_where.values.append(content);
                    db->delDataPS("reservations", sql_where);
                    switch(content[0].unicode()){
                    case 'F': db->delDataPS("flight_reservations", sql_where_resv); break;
                    case 'H': db->delDataPS("hotel_reservations", sql_where_resv); break;
                    case 'B': db->delDataPS("bus_reservations", sql_where_resv); break;
                    }
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
        db->getDataPS("flights", data);
        return data;
    }
    case 1:{
        QList<QHash<QString,QString>> data;
        db->getDataPS("hotels", data);
        return data;
    }
    case 2:{
        QList<QHash<QString,QString>> data;
        db->getDataPS("bus", data);
        return data;
    }
    case 3: {
        QList<QHash<QString,QString>> data;
        db->getDataPS("customers", data);
        return data;
    }
    case 4:{
        QList<QHash<QString,QString>> data;
        db->getDataPS("reservations", data);
        bool ok;
        for(auto &d :data){
            QHash<QString,QString> dataContent;
            MysqlDb::SqlWhere sql_where_resv;
            sql_where_resv.str = "where resvKey = ?";
            sql_where_resv.values.append(d["resvKey"]);
            switch(d["resvType"].toInt(&ok, 10)){
            case 0: {
                db->getDataPS("flight_reservations", dataContent, sql_where_resv);
                if(dataContent["flightNum"] != ""){
                    d.insert("resvContent", dataContent["flightNum"]);
                } else {
                    d.insert("resvContent", "不存在");
                }
                break;
            }
            case 1: {
                db->getDataPS("hotel_reservations", dataContent, sql_where_resv);
                if(dataContent["location"] != ""){
                    d.insert("resvContent", dataContent["location"]);
                } else {
                    d.insert("resvContent", "不存在");
                }
                break;
            }
            case 2: {
                db->getDataPS("bus_reservations", dataContent, sql_where_resv);
                if(dataContent["location"] != ""){
                    d.insert("resvContent", dataContent["location"]);
                } else {
                    d.insert("resvContent", "不存在");
                }
                break;
            }
            }
        }
        return data;
    }
    default: {
        QMessageBox::critical(NULL, "错误", "未知数据请求", QMessageBox::Yes);
        return QList<QHash<QString,QString>>();
    }
    }

}

QList<QHash<QString,QString>> Server::getData(User user){
    QList<QHash<QString,QString>> data;
    MysqlDb::SqlWhere sql_where;
    sql_where.str = "where custName = ?";
    sql_where.values.append(user.getName());
    db->getDataPS("reservations", data, sql_where);
    bool ok;
    for(auto &d :data){
        QHash<QString,QString> dataContent;
        MysqlDb::SqlWhere sql_where_resv;
        sql_where_resv.str = "where resvKey = ?";
        sql_where_resv.values.append(d["resvKey"]);
        switch(d["resvType"].toInt(&ok, 10)){
        case 0: {
            db->getDataPS("flight_reservations", dataContent, sql_where_resv);
            if(dataContent["flightNum"] != ""){
                d.insert("resvContent", dataContent["flightNum"]);
            } else {
                d.insert("resvContent", "不存在");
            }
            break;
        }
        case 1: {
            db->getDataPS("hotel_reservations", dataContent, sql_where_resv);
            if(dataContent["location"] != ""){
                d.insert("resvContent", dataContent["location"]);
            } else {
                d.insert("resvContent", "不存在");
            }
            break;
        }
        case 2: {
            db->getDataPS("bus_reservations", dataContent, sql_where_resv);
            if(dataContent["location"] != ""){
                d.insert("resvContent", dataContent["location"]);
            } else {
                d.insert("resvContent", "不存在");
            }
            break;
        }
        }
    }
    return data;
}

QList<QHash<QString,QString>> Server::getData(int table_flag, int attribute_flag, QString search_content){
    QString table;
    QString attribute;
    MysqlDb::SqlWhere sql_where;
    switch(table_flag){
    case 0: {
        table = "flights";
        switch(attribute_flag){
        case 0: attribute = "flightNum"; break;
        case 1: attribute = "FromCity"; break;
        case 2: attribute = "ArivCity"; break;
        }
        sql_where.str = "where " + attribute + " LIKE ?";
        sql_where.values.append("%" + search_content + "%");
        break;
    }
    case 1: {
        table = "hotels";
        switch(attribute_flag){
        case 0: attribute = "location"; break;
        }
        sql_where.str = "where " + attribute + " LIKE ?";
        sql_where.values.append("%" + search_content + "%");
        break;
    }
    case 2: {
        table = "bus";
        switch(attribute_flag){
        case 0: attribute = "location"; break;
        }
        sql_where.str = "where " + attribute + " LIKE ?";
        sql_where.values.append("%" + search_content + "%");
        break;
    }
    case 3: {
        table = "customers";
        switch(attribute_flag){
        case 0: attribute = "custName"; break;
        }
        sql_where.str = "where " + attribute + " LIKE ?";
        sql_where.values.append("%" + search_content + "%");
        break;
    }
    case 4: {
        table = "reservations";
        switch(attribute_flag){
        case 0: attribute = "resvKey"; break;
        case 1: attribute = "custName"; break;
        case 2: attribute = "flightNum"; break;
        case 3: attribute = "location"; break;
        case 4: attribute = "location"; break;
        }
        sql_where.str = "where " + attribute + " LIKE ?";
        sql_where.values.append("%" + search_content + "%");
        break;
    }
    }
    QList<QHash<QString,QString>> data;
    if(table_flag  < 4){
        db->getDataPS(table, data, sql_where);
    } else {
        if(attribute_flag < 2){
            db->getDataPS(table, data, sql_where);
            bool ok;
            for(auto &d :data){
                QHash<QString,QString> dataContent;
                MysqlDb::SqlWhere sql_where_resv;
                sql_where_resv.str = "where resvKey = ?";
                sql_where_resv.values.append(d["resvKey"]);
                switch(d["resvType"].toInt(&ok, 10)){
                case 0: {
                    db->getDataPS("flight_reservations", dataContent, sql_where_resv);
                    if(dataContent["flightNum"] != ""){
                        d.insert("resvContent", dataContent["flightNum"]);
                    } else {
                        d.insert("resvContent", "不存在");
                    }
                    break;
                }
                case 1: {
                    db->getDataPS("hotel_reservations", dataContent, sql_where_resv);
                    if(dataContent["location"] != ""){
                        d.insert("resvContent", dataContent["location"]);
                    } else {
                        d.insert("resvContent", "不存在");
                    }
                    break;
                }
                case 2: {
                    db->getDataPS("bus_reservations", dataContent, sql_where_resv);}
                    if(dataContent["location"] != ""){
                        d.insert("resvContent", dataContent["location"]);
                    } else {
                        d.insert("resvContent", "不存在");
                    }
                    break;
                }
            }
        } else {
            switch(attribute_flag){
            case 2: db->getDataPS("flight_reservations", data, sql_where); break;
            case 3: db->getDataPS("hotel_reservations", data, sql_where); break;
            case 4: db->getDataPS("bus_reservations", data, sql_where); break;
            }
            for(auto &d : data){
                QHash<QString,QString> data_all;
                MysqlDb::SqlWhere sql_where_resv;
                sql_where_resv.str = "where resvKey = ?";
                sql_where_resv.values.append(d["resvKey"]);
                db->getDataPS("reservations", data_all, sql_where_resv);
                d.insert(data_all);
                switch(attribute_flag){
                case 2: d.insert("resvContent", d["flightNum"]); break;
                case 3: d.insert("resvContent", d["location"]); break;
                case 4: d.insert("resvContent", d["location"]); break;
                }
            }
        }
    }
    return data;
}

QHash<QString,QString> Server::getData(int flag, QString key){
    QString table;
    MysqlDb::SqlWhere sql_where;
    switch(flag){
    case 0: table = "flights"; sql_where.str = "where flightNum = ?"; break;
    case 1: table = "hotels"; sql_where.str = "where location = ?"; break;
    case 2: table = "bus"; sql_where.str = "where location = ?"; ; break;
    case 3: table = "customers"; sql_where.str = "where custName = ?"; break;
    case 4: table = "reservations"; sql_where.str = "where resvKey = ?"; break;
    }
    sql_where.values.append(key);
    QHash<QString,QString> data;
    db->getDataPS(table, data, sql_where);
    if(flag == 4){
        bool ok;
        QHash<QString,QString> dataContent;
        MysqlDb::SqlWhere sql_where_resv;
        sql_where_resv.str = "where resvKey = ?";
        sql_where_resv.values.append(data["resvKey"]);
        switch(data["resvType"].toInt(&ok, 10)){
        case 0: {
            db->getDataPS("flight_reservations", dataContent, sql_where_resv);
            if(dataContent["flightNum"] != ""){
                data.insert("resvContent", dataContent["flightNum"]);
            } else {
                data.insert("resvContent", "不存在");
            }
            break;
        }
        case 1: {
            db->getDataPS("hotel_reservations", dataContent, sql_where_resv);
            if(dataContent["location"] != ""){
                data.insert("resvContent", dataContent["location"]);
            } else {
                data.insert("resvContent", "不存在");
            }
            break;
        }
        case 2: {
            db->getDataPS("bus_reservations", dataContent, sql_where_resv);
            if(dataContent["location"] != ""){
                data.insert("resvContent", dataContent["location"]);
            } else {
                data.insert("resvContent", "不存在");
            }
            break;
        }
        }
    }
    return data;
}

User* Server::getCurrentUser(){
    return currentUser;
}

Server::Status Server::getStatus(){
    return status;
}

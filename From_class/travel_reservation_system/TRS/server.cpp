#include "server.h"
#include "setdbdialog.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QRegularExpression>

Server::Server(QString host, int port, QString name, QString password, QString database) {
    if(!startServer(host, port, name, password, database)){
        QTimer::singleShot(0, this, [this](){ emit exitSent(1); });
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
            case QMessageBox::No: {delete msg; delete sdd; ; return false;}
            default: {delete msg; delete sdd; emit ; return false;}
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
            if(tempUser.getName() == user["custName"] && tempUser.getId() == user["custID"]){
                currentUser = new User(tempUser.getName(), tempUser.getId(), User::USER);
                emit userSent(currentUser);
                return true;
            } else{
                QMessageBox::critical(NULL, "critical", "ID错误", QMessageBox::Yes);
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
        if(tempUser.getId() == "1234"){
            currentUser = new User(User::ROOT);
            emit userSent(currentUser);
            return true;
        } else{
            QMessageBox::critical(NULL, "critical", "ID错误", QMessageBox::Yes);
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
    delete currentUser;
    currentUser = new User(User::VISITOR);
    emit userSent(currentUser);
}

void Server::signup(User tempUser){
    if(tempUser.getName() != "" || tempUser.getId() != ""){
        QHash<QString, QString> user;
        user.insert("custName", tempUser.getName());
        user.insert("custID", tempUser.getId());
        if(db->addData("customers", user)){
            QMessageBox::information(NULL, "成功", "注册成功", QMessageBox::Yes);
        } else{
            QMessageBox::information(NULL, "失败", "注册失败", QMessageBox::Yes);
        }
    } else{
        QMessageBox::critical(NULL, "错误", "用户名和ID不能为空", QMessageBox::Yes);
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

#include "servor.h"
#include "setdbdialog.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QRegularExpression>

Servor::Servor(QString host, int port, QString name, QString password, QString database) {
    if(!startServor(host, port, name, password, database)){
        QTimer::singleShot(0, this, [this](){ emit exitSent(1); });
    }
}

bool Servor::startServor(QString host, int port, QString name, QString password, QString database){
    connectToDatabase(host, port, name, password, database);
    if(db == NULL){
        QMessageBox *msg = new QMessageBox(QMessageBox::Critical, "错误", "数据库连接出错，是否重新连接数据库？", QMessageBox::Yes | QMessageBox::No, NULL);
        SetDbDialog *sdd = new SetDbDialog(host, port, name, password, database);
        while(db == NULL){
            switch(msg->exec()){
            case QMessageBox::Yes: {
                sdd->exec();
                connect(sdd, SIGNAL(dbSetSent(QString, int, QString, QString, QString, bool)), this, SLOT(dbSetReceive(QString, int, QString, QString, QString, bool)));
                sdd->exec();
                connect(sdd, SIGNAL(dbSetSent(QString, int, QString, QString, QString, bool)), this, SLOT(dbSetReceive(QString, int, QString, QString, QString, bool)));
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

void Servor::dbSetReceive(QString host, int port, QString name, QString password, QString database, bool flag){
    if(flag){
        connectToDatabase(host, port, name, password, database);
    }
}

bool Servor::connectToDatabase(QString host, int port, QString name, QString password, QString database){
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

bool Servor::login(QString name, QString id, int flag){
    if(currentUser != NULL){
        delete currentUser;
    }
    if(flag == User::USER){
        QHash<QString, QString> user;
        QString sqlWhere = "where custName = '" + name + "'";
        bool jud = db->getData("customers", user, sqlWhere);
        if(jud){
            if(name == user["custName"] && id == user["custID"]){
                currentUser = new User(name, id, User::USER);
                return true;
            } else{
                QMessageBox::critical(NULL, "critical", "ID错误", QMessageBox::Yes);
                currentUser = new User(User::VISITOR);
                return false;
            }
        } else{
            QMessageBox::critical(NULL, "critical", "用户名错误", QMessageBox::Yes);
            currentUser = new User(User::VISITOR);
            return false;
        }
    } else if(flag == User::ROOT){
        if(id == "1234"){
            currentUser = new User("root", id, User::ROOT);
            return true;
        } else{
            QMessageBox::critical(NULL, "critical", "ID错误", QMessageBox::Yes);
            currentUser = new User(User::VISITOR);
            return false;
        }
    } else{
        QMessageBox::critical(NULL, "critical", "出错啦：Login flag is wrong", QMessageBox::Yes);
        currentUser = new User(User::VISITOR);
        return false;
    }
}

void Servor::logout(){
    delete currentUser;
    currentUser = new User(User::VISITOR);
}

void Servor::signup(QString name, QString id){
    if(name != "" || id != ""){
        QHash<QString, QString> user;
        user.insert("custName", name);
        user.insert("custID", id);
        if(db->addData("customers", user)){
            QMessageBox::information(NULL, "成功", "注册成功", QMessageBox::Yes);
        } else{
            QMessageBox::information(NULL, "失败", "注册失败", QMessageBox::Yes);
        }
    } else{
        QMessageBox::critical(NULL, "错误", "用户名和ID不能为空", QMessageBox::Yes);
    }
}

QList<QHash<QString,QString>> Servor::getData(int flag){
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

User* Servor::getCurrentUser(){
    return currentUser;
}

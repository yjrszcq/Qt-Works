#include "servor.h"
#include "setdbdialog.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QTimer>
#include <QEventLoop>

Servor::Servor(QString host, int port, QString name, QString password, QString database) {
    connectToDatabase(host, port, name, password, database);
    if(db == NULL){
        QMessageBox *msg = new QMessageBox(QMessageBox::Critical, "错误", "数据库连接出错，是否重新连接数据库？", QMessageBox::Yes | QMessageBox::No, NULL);
        SetDbDialog *sdd = new SetDbDialog(host, port, name, password, database);
        while(db == NULL){
            switch(msg->exec()){
            case QMessageBox::Yes: {
                sdd->exec();
                connect(sdd, SIGNAL(dataSent(QString, int, QString, QString, QString, bool)), this, SLOT(dataReceive(QString, int, QString, QString, QString, bool)));
                sdd->exec();
                connect(sdd, SIGNAL(dataSent(QString, int, QString, QString, QString, bool)), this, SLOT(dataReceive(QString, int, QString, QString, QString, bool)));
                break;
            }
            case QMessageBox::No: QCoreApplication::exit(0); break;
            default : QCoreApplication::exit(0); break;
            }
        }
        delete msg;
        delete sdd;
    }
    currentUser = NULL;
}

void Servor::dataReceive(QString host, int port, QString name, QString password, QString database, bool flag){
    if(flag){
        connectToDatabase(host, port, name, password, database);
    } else{
        QCoreApplication::exit(0);
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
        QString sqlWhere = "where custName = " + name;
        bool jud = db->getData("customers", user, sqlWhere);
        if(jud){
            if(name == user["custName"] && id == user["custID"]){
                currentUser = new User(name, id, User::USER);
                return true;
            } else{
                QMessageBox::critical(NULL, "critical", "ID错误", QMessageBox::Yes);
                return false;
            }
        } else{
            QMessageBox::critical(NULL, "critical", "用户名错误", QMessageBox::Yes);
            return false;
        }
    } else if(flag == User::ROOT){
        if(id == "1234"){
            currentUser = new User("root", id, User::ROOT);
            return true;
        } else{
            QMessageBox::critical(NULL, "critical", "ID错误", QMessageBox::Yes);
            return false;
        }
    } else{
        QMessageBox::critical(NULL, "critical", "出错啦：Login flag is wrong", QMessageBox::Yes);
        return false;
    }
}

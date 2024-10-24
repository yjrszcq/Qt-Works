#include "mysqldb.h"
#include <QDebug>
#include <QUuid>

MysqlDb::MysqlDb() {
    qDebug()<<"support drivers:"<<QSqlDatabase::drivers();
}

MysqlDb::~MysqlDb() {
    disConnectSql();
}

//打开连接
bool MysqlDb::connectSql(const QString &dbName) {
    mdbName_= dbName;
    db = QSqlDatabase::database(connectionName);
    if(!db.isValid()) {
        QUuid qUuid = QUuid::createUuid();
        QString strUuid = qUuid.toString();
        connectionName = QString("mysql-%1").arg(strUuid);
        db = QSqlDatabase::addDatabase("QMYSQL",connectionName);
        db.setHostName(mhost_); //根据实际情况设置主机名
        db.setPort(mport_);
        db.setDatabaseName(dbName);
        db.setUserName(muser_); //根据实际情况设置用户名
        db.setPassword(mpwd_); //根据实际情况设置密码
        db.setConnectOptions("MYSQL_OPT_RECONNECT=1"); // 支持断线重连
        if (!db.open()) {
            qWarning("Failed to open database: %s", qPrintable(db.lastError().text()));
            return false;
        }
    }
    return true;
}
//打开连接
bool MysqlDb::connectSql(const QString &host, int port, const QString &dbName, const QString &userName, const QString &password) {
    mhost_= host;
    mport_= port;
    mdbName_= dbName;
    muser_= userName;
    mpwd_ = password;
    db = QSqlDatabase::database(connectionName);
    if(!db.isValid()) {
        QUuid qUuid = QUuid::createUuid();
        QString strUuid = qUuid.toString();
        connectionName = QString("mysql-%1").arg(strUuid);
        db = QSqlDatabase::addDatabase("QMYSQL",connectionName);
        db.setHostName(host); //根据实际情况设置主机名
        db.setPort(port);
        db.setDatabaseName(dbName);
        db.setUserName(userName); //根据实际情况设置用户名
        db.setPassword(password); //根据实际情况设置密码
        db.setConnectOptions("MYSQL_OPT_RECONNECT=1"); // 支持断线重连
        if (!db.open()) {
            qWarning("Failed to open database: %s", qPrintable(db.lastError().text()));
            return false;
        }
    }
    return true;
}

//关闭连接
bool MysqlDb::disConnectSql() {
    db = QSqlDatabase::database(connectionName);
    if(!db.isValid()) {
        return true;
    }
    db.close();
    QSqlDatabase::removeDatabase(connectionName);
    connectionName = "";
    return true;
}

//错误打印
void MysqlDb::errorSql(QString sql) {
    errorSqlText = sql;
    qCritical("%s", qPrintable(errorSqlText));
}

//获取错误的数据库语句
QString MysqlDb::getErrorSql() {
    if(connectionName.isEmpty()) {
        return "db not setting";
    }
    return errorSqlText;
}

void MysqlDb::setMdbName(const QString &mdbName)
{
    mdbName_ = mdbName;
}

void MysqlDb::setMpwd(const QString &mpwd)
{
    mpwd_ = mpwd;
}

void MysqlDb::setMuser(const QString &muser)
{
    muser_ = muser;
}

void MysqlDb::setMhost(const QString &mhost)
{
    mhost_ = mhost;
}

void MysqlDb::setMport(int mport)
{
    mport_ = mport;
}

//执行sql语句，不获取结果
bool MysqlDb::queryExec( QString queryStr) {
    if(connectionName.isEmpty()) {
        if(!connectSql(mhost_,mport_,mdbName_,muser_,mpwd_)) {
            return false;
        }
    }
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(!query.exec(queryStr)) {
        errorSql(queryStr);
        return false;
    }
    return true;
}

//执行sql语句，并获取结果
bool MysqlDb::queryExec( QString queryStr, QList<QHash<QString, QString>> &data) {
    data.clear();
    if(connectionName.isEmpty()) {
        if(!connectSql(mhost_,mport_,mdbName_,muser_,mpwd_)) {
            return false;
        }
    }
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(!query.exec(queryStr)) {
        errorSql(queryStr);
        return false;
    }
    QSqlRecord rec = query.record();
    while(query.next()) {
        QHash<QString, QString> rowData;
        for(int i = 0; i < rec.count(); i++) {
            QVariant::Type ty = query.value(i).type();
            if(QVariant::Type::Date == ty) {
                QDate temp = query.value(i).toDate();
                rowData[rec.fieldName(i)] = temp.toString("yyyy-MM-dd");
            } else if(QVariant::Type::Time == ty) {
                QTime temp = query.value(i).toTime();
                rowData[rec.fieldName(i)] = temp.toString("hh:mm:ss");
            } else if(QVariant::Type::DateTime == ty) {
                QDateTime temp = query.value(i).toDateTime();
                rowData[rec.fieldName(i)] = temp.toString("yyyy-MM-dd hh:mm:ss");
            } else {
                rowData[rec.fieldName(i)] = query.value(i).toString();
            }
        }
        data.append(rowData);
    }
    return true;
}

//获取数据
bool MysqlDb::getData(QString tableName, QHash<QString, QString> &data, QString sqlWhere) {
    data.clear();
    QList<QHash<QString, QString>> dataList;
    if(!getData(tableName, dataList, sqlWhere)) {
        return false;
    }
    if(dataList.count() > 0) {
        data = dataList[0];
    }
    return true;
}

//获取数据
bool MysqlDb::getData( QString tableName, QList<QHash<QString, QString>> &data, QString sqlWhere) {
    QString queryStr = "select * from " + tableName;
    if(!sqlWhere.isEmpty()) {
        queryStr += " " + sqlWhere;
    }
    return queryExec(queryStr, data);
}

//获取数据
bool MysqlDb::getData(QString tableName, QHash<QString, QString> columndata, QList<QHash<QString, QString>> &data, QString sqlWhere) {
    QString colunmStr;
    if(columndata.count() == 0) {
        colunmStr = "*";
    } else {
        QStringList keys = columndata.keys();
        for(auto key : keys) {
            QString column = QString("%1 AS %2").arg(key).arg(columndata[key]);
            if(!colunmStr.isEmpty()) {
                colunmStr += ",";
            }
            colunmStr += column;
        }
    }
    QString queryStr = QString("SELECT %1 FROM %2 %3").arg(colunmStr).arg(tableName).arg(sqlWhere);
    return queryExec(queryStr, data);
}

//增加
bool MysqlDb::addData(QString tableName, QHash<QString, QString> data) {
    if(data.isEmpty()) {
        return false;
    }
    QString queryStr = "INSERT INTO " + tableName + " ";
    QString fieldStr = "(", valueStr = "VALUES(";
    QHash<QString, QString>::iterator it;
    for(it = data.begin(); it != data.end(); ++it) {
        fieldStr += it.key() + ",";
        valueStr += "'" + it.value() + "',";
    }
    fieldStr = fieldStr.left(fieldStr.length() - 1);
    valueStr = valueStr.left(valueStr.length() - 1);
    fieldStr += ")";
    valueStr += ")";
    queryStr += fieldStr + " " + valueStr;
    return queryExec(queryStr);
}

//删除
bool MysqlDb::delData(QString tableName, QString sqlWhere) {
    QString queryStr = "DELETE FROM " + tableName;
    if(!sqlWhere.isEmpty()) {
        queryStr += " " + sqlWhere;
    }
    return queryExec(queryStr);
}

//修改
bool MysqlDb::updateData( QString tableName, QHash<QString, QString> data, QString sqlWhere) {
    QString queryStr = "UPDATE " + tableName + " ";
    QHash<QString, QString>::iterator it;
    QString setStr = "SET ";
    for(it = data.begin(); it != data.end(); ++it) {
        setStr += it.key() + "='" + it.value() + "'";
        setStr += ",";
    }
    setStr = setStr.left(setStr.length() - 1);
    queryStr += setStr;
    if(!sqlWhere.isEmpty()) {
        queryStr += " " + sqlWhere;
    }
    return queryExec(queryStr);
}

bool MysqlDb::transaction() {
    if(connectionName.isEmpty()) {
        return false;
    }
    return db.transaction();
}

bool MysqlDb::commit() {
    if(connectionName.isEmpty()) {
        return false;
    }
    return db.commit();
}
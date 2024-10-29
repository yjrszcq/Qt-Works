# Qt中使用MySQL数据库详解，好用的模块类封装

本文为以下来源的**改进版**

来源：```https://blog.csdn.net/yyz_1987/article/details/140106546```

`本文将详细介绍如何在Qt应用程序中集成MySQL数据库，并封装实现好用的mysql数据库操作类。包括环境准备、连接数据库、执行查询及异常处理等关键步骤，同时包含mysql驱动的编译。分享给有需要的小伙伴，喜欢的可以点击收藏。` 

## 目录

```
环境准备
项目配置
简单使用
简单示例
模块类封装
    .cpp代码
    .h代码
如何使用
附qsqlmysql库的构建
    前提条件
    构建步骤
        1. 查找Qt的安装路径和编译器
        2. 设置环境变量
        3. 获取MySQL开发库
        4. 构建qsqlmysql插件
            Windows:
            Linux:
        5. 将编译好的插件拷贝到合适的位置
    注意事项
其他资源
```

Qt提供了QtSql模块来进行独立于平台的数据库操作，这里的“平台”既包括操作系统平台，也包括各个数据库平台。Qt使用一个QDatabase表示一个数据库连接。在底层，Qt使用不同的驱动程序来与不同的数据库API进行交互。

通常Qt只默认搭载了QSqlLite驱动程序，如果需要使用其他数据库，需要下载相应的数据库驱动，如mysql的为 qsqlmysql.dll，同时还需要mysql的客户端库libmysql.dll。

在连接数据库之前可以使用QSqlDatabase::drivers()查看本机Qt已经支持的数据库驱动。

## 环境准备

1.安装MySQL数据库：首先确保你的系统中安装了MySQL服务器，并创建好数据库和表结构。

2.安装Qt开发环境：安装Qt Creator及Qt库，确保包含SQL驱动模块。

注意：Qt默认并不包括MySQL驱动，需要手动构建。在QT安装目录（如Qt5.12.11\5.12.11\msvc2015_64\plugins\sqldrivers）里找，是否有qsqlmysql.dll和qsqlmysqld.dll.

如果没有则需要基于QT源码从新构建，构建好后把qsqlmysql.dll放入plugins\sqldrivers目录中。如果不存在该库，则程序执行会报QSqlDatabase: QMYSQL driver not loaded的错误。

如果要构建QMYSQL，需安装Qt源代码，并确保你的系统中安装了MySQL服务器或至少安装了MySQL Connector/C++，因为构建过程需要MySQL的头文件和库文件。

可以通过以下方式查看支持哪些驱动：

```
qDebug()<<"support drivers:"<<QSqlDatabase::drivers();
```

3.安装MySQL的c/c++的Connector（MySQL客户端库）：对于Qt 5.12及以上版本，MySQL驱动可能已内置，但若缺失，需下载MySQL Connector/C++并安装，确保Qt能找到libmysql动态库。
(libmysql.dll动态库。链接：```https://downloads.mysql.com/archives/c-c/```)

将压缩包解压，将lib文件夹下的libmysql.dll和libmysql.lib文件拷贝到Qt的安装目录的bin文件夹下即可。 

## 项目配置

在你的Qt项目文件（.pro）中添加如下行以启用SQL模块：

```
QT += sql
```

## 简单使用

编写代码连接数据库：

```Cpp
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
 
void connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("testdb");
    db.setUserName("root");
    db.setPassword("password");
 
    if (!db.open()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
    } else {
        qDebug() << "Connected to database!";
    }
}
```
 

## 简单示例

```Cpp
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
 
void connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("testdb");
    db.setUserName("root");
    db.setPassword("password");
 
    if (!db.open()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
    } else {
        qDebug() << "Connected to database!";
    }
}
 
bool createTable() {
    QSqlQuery query;
    bool success = query.exec("CREATE TABLE person (id INT PRIMARY KEY, name VARCHAR(40))");
    if (!success) {
        qDebug() << "Failed to create table:" << query.lastError().text();
    }
    return success;
}
 
bool insertRecord(int id, const QString &name) {
    QSqlQuery query;
    query.prepare("INSERT INTO person (id, name) VALUES (:id, :name)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    bool success = query.exec();
    if (!success) {
        qDebug() << "Failed to insert record:" << query.lastError().text();
    }
    return success;
}
 
void queryRecords() {
    QSqlQuery query("SELECT id, name FROM person");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        qDebug() << id << name;
    }
}
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
 
    connectToDatabase();
    createTable();
    insertRecord(1, "Alice");
    insertRecord(2, "Bob");
    queryRecords();
 
    return a.exec();
}
```

## 模块类封装

直接使用不太好用，这里做一个模块类封装，变得更好用啦，接口变得简单清晰。如果不封装，则原始是使用大概如下：

```Cpp
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    query.prepare("insert into test(name,age) values(:nameL,:ageL)");
    QStringList namelist;
    namelist<<"Tt"<<"Pp"<<"Kk";
    query.bindValue(":nameL",namelist);
    QVariantList agelist;
    agelist<<40<<50<<60;
    query.bindValue(":ageL",agelist);
    if(!query.execBatch()){
        qDebug()<<"数据插入失败： "<<query.lastError().text();
    }else{
        qDebug()<<"数据插入成功！";
    }
```

可以看到使用比较繁琐，封装后的模块代码如下： 

### .cpp代码
```Cpp
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

//影响行数
long int MysqlDb::getNumRowsAffected(){
    return numRowsAffected;
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
    long int nra = query.numRowsAffected();
    if(nra >= 0){
        numRowsAffected = nra;
    } else{
        numRowsAffected = 0;
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
    long int nra = query.numRowsAffected();
    if(nra >= 0){
        numRowsAffected = nra;
    } else{
        numRowsAffected = 0;
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

//预编译sql
bool MysqlDb::queryPrepare(QSqlQuery &query, QString queryStr){
    //预编译
    if(!query.prepare(queryStr)){
        qDebug() << "预编译失败";
        errorSql(queryStr);
        return false;
    }
    return true;

}

//预编译sql，含参数绑定
bool MysqlDb::queryPrepare(QSqlQuery &query, QString queryStr, QList<QString> values){
    //预编译
    if(!queryPrepare(query, queryStr)){
        return false;
    }
    //预编译参数绑定
    for(auto v : values){
        bool ok_int, ok_double;
        int iv = v.toInt(&ok_int, 10);
        double dv = v.toDouble(&ok_double);
        if(ok_int){
            query.addBindValue(iv);
        } else if(ok_double){
            query.addBindValue(dv);
        } else{
            query.addBindValue(v);
        }
    }
    return true;
}

//执行sql语句，不获取结果
bool MysqlDb::queryExecPS(QSqlQuery &query, QString queryStr){
    if(connectionName.isEmpty()) {
        if(!connectSql(mhost_,mport_,mdbName_,muser_,mpwd_)) {
            return false;
        }
    }
    if(!query.exec()) {
        errorSql(queryStr);
        return false;
    }
    long int nra = query.numRowsAffected();
    if(nra >= 0){
        numRowsAffected = nra;
    } else{
        numRowsAffected = 0;
    }
    return true;
}

//执行sql语句，并获取结果
bool MysqlDb::queryExecPS(QSqlQuery &query, QString queryStr,QList<QHash<QString,QString>> &data){
    data.clear();
    if(connectionName.isEmpty()) {
        if(!connectSql(mhost_,mport_,mdbName_,muser_,mpwd_)) {
            return false;
        }
    }
    if(!query.exec()) {
        errorSql(queryStr);
        return false;
    }
    long int nra = query.numRowsAffected();
    if(nra >= 0){
        numRowsAffected = nra;
    } else{
        numRowsAffected = 0;
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
bool MysqlDb::getDataPS(QString tableName,QHash<QString,QString> &data,SqlWhere sqlWhere){
    data.clear();
    QList<QHash<QString, QString>> dataList;
    if(!getDataPS(tableName, dataList, sqlWhere)) {
        return false;
    }
    if(dataList.count() > 0) {
        data = dataList[0];
    }
    return true;
}

//获取数据
bool MysqlDb::getDataPS(QString tableName,QList<QHash<QString,QString>> &data, SqlWhere sqlWhere){
    QString queryStr = "select * from " + tableName;
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(!sqlWhere.str.isEmpty()) {
        queryStr += " " + sqlWhere.str;
        if(queryPrepare(query, queryStr, sqlWhere.values)){
            return queryExecPS(query, queryStr, data);
        }
    } else{
        if(queryPrepare(query, queryStr)){
            return queryExecPS(query, queryStr, data);
        }
    }
    return false;
}

//获取数据
bool MysqlDb::getDataPS(QString tableName,QHash<QString,QString> columndata,QList<QHash<QString,QString>> &data,SqlWhere sqlWhere){
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
    QString queryStr = QString("SELECT %1 FROM %2 %3").arg(colunmStr).arg(tableName).arg(sqlWhere.str);
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(queryPrepare(query, queryStr, sqlWhere.values)){
        return queryExecPS(query, queryStr, data);
    }
    return false;
}

//增加
bool MysqlDb::addDataPS(QString tableName,QHash<QString,QString> data){
    if(data.isEmpty()) {
        return false;
    }
    QString queryStr = "INSERT INTO " + tableName + " ";
    QString fieldStr = "(", valueStr = "VALUES(";
    QList<QString> values;
    QHash<QString, QString>::iterator it;
    for(it = data.begin(); it != data.end(); ++it) {
        fieldStr += it.key() + ",";
        valueStr += "?,";
        values.append(it.value());
    }
    fieldStr = fieldStr.left(fieldStr.length() - 1);
    valueStr = valueStr.left(valueStr.length() - 1);
    fieldStr += ")";
    valueStr += ")";
    queryStr += fieldStr + " " + valueStr;
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(queryPrepare(query, queryStr, values)){
        return queryExecPS(query, queryStr);
    }
    return false;
}

//删除
bool MysqlDb::delDataPS(QString tableName, SqlWhere sqlWhere){
    QString queryStr = "DELETE FROM " + tableName;
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(!sqlWhere.str.isEmpty()) {
        queryStr += " " + sqlWhere.str;
        if(queryPrepare(query, queryStr, sqlWhere.values)){
            return queryExecPS(query, queryStr);
        }
    } else{
        if(queryPrepare(query, queryStr)){
            return queryExecPS(query, queryStr);
        }
    }
    return false;
}

//修改
bool MysqlDb::updateDataPS(QString tableName,QHash<QString,QString> data, SqlWhere sqlWhere){
    QString queryStr = "UPDATE " + tableName + " ";
    QHash<QString, QString>::iterator it;
    QString setStr = "SET ";
    QList<QString> values;
    for(it = data.begin(); it != data.end(); ++it) {
        setStr += it.key() + "= ?";
        setStr += ",";
        values.append(it.value());
    }
    setStr = setStr.left(setStr.length() - 1);
    queryStr += setStr;
    QSqlQuery query(QSqlDatabase::database(connectionName, true));
    if(!sqlWhere.str.isEmpty()) {
        queryStr += " " + sqlWhere.str;
        values.append(sqlWhere.values);
        if(queryPrepare(query, queryStr, values)){
            return queryExecPS(query, queryStr);
        }
    } else{
        if(queryPrepare(query, queryStr, values)){
            return queryExecPS(query, queryStr);
        }
    }
    return false;
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
```
### .h代码
```Cpp
#ifndef MYSQLDB_H
#define MYSQLDB_H

#include <QDir>
#include <QDate>
#include <QDateTime>
#include <QFileInfo>
#include <QString>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

class MysqlDb
{
public:
    MysqlDb();
    ~MysqlDb();
public:
    typedef struct SqlWhere{
        QString str;
        QList<QString> values;
        SqlWhere() = default;
    }SqlWhere;

    bool connectSql(const QString &dbName);//打开连接
    bool connectSql(const QString &host, int port, const QString &dbName, const QString &userName, const QString &password);//打开连接
    bool disConnectSql();//关闭连接

    //无sql预编译
    bool queryExec(QString sqlStr);//执行sql语句，不获取结果
    bool queryExec(QString sqlStr,QList<QHash<QString,QString>> &data);//执行sql语句，并获取结果
    bool getData(QString tableName,QHash<QString,QString> &data,QString sqlWhere=""); //获取数据
    bool getData(QString table,QList<QHash<QString,QString>> &data,QString sqlWhere=""); //获取数据
    bool getData(QString tableName,QHash<QString,QString> columndata,QList<QHash<QString,QString>> &data,QString sqlWhere=""); //获取数据
    bool addData(QString tableName,QHash<QString,QString> data);//增加
    bool delData(QString tableName,QString sqlWhere);//删除
    bool updateData(QString tableName,QHash<QString,QString> data,QString sqlWhere="");//修改

    //有sql预编译
    bool queryPrepare(QSqlQuery &query, QString sqlStr);//预编译sql
    bool queryPrepare(QSqlQuery &query, QString sqlStr, QList<QString> values);//预编译sql，含参数绑定
    bool queryExecPS(QSqlQuery &query, QString sqlStr);//执行sql语句，不获取结果
    bool queryExecPS(QSqlQuery &query, QString sqlStr,QList<QHash<QString,QString>> &data);//执行sql语句，并获取结果
    bool getDataPS(QString tableName,QHash<QString,QString> &data,SqlWhere sqlWhere = SqlWhere()); //获取数据
    bool getDataPS(QString table,QList<QHash<QString,QString>> &data,SqlWhere  sqlWhere = SqlWhere()); //获取数据
    bool getDataPS(QString tableName,QHash<QString,QString> columndata,QList<QHash<QString,QString>> &data,SqlWhere sqlWhere = SqlWhere()); //获取数据
    bool addDataPS(QString tableName,QHash<QString,QString> data);//增加
    bool delDataPS(QString tableName, SqlWhere sqlWhere = SqlWhere());//删除
    bool updateDataPS(QString tableName,QHash<QString,QString> data, SqlWhere sqlWhere = SqlWhere());//修改

    bool transaction();
    bool commit();
    long int getNumRowsAffected();
    QString getErrorSql();//获取错误的数据库语句

    void setMhost(const QString &mhost);
    void setMport(int mport);
    void setMdbName(const QString &mdbName);
    void setMuser(const QString &muser);
    void setMpwd(const QString &mpwd);

private:
    QSqlDatabase db;
    QString connectionName="";
    QString errorSqlText;//错误语句
    QString mdbName_="";
    QString mhost_ = "localhost";
    int mport_ = 3306;
    QString muser_="";
    QString mpwd_="";
    long int numRowsAffected=0;

private:
    void errorSql(QString sql);//错误打印
};

#endif // MYSQLDB_H
```

## 如何使用

### 无SQL预编译版
```Cpp
void MainWindow::on_btn_test_clicked()
{
    MysqlDb* db = new mysqlDb();
    db->setMhost("111.178.126.10");
    db->setMuser("xxxxxx");
    db->setMpwd("xxxxxx");
    bool ret = db->connectSql("test");
    if(ret){
        qDebug("connect ok");

        //读取数据
        QList<QHash<QString, QString>> data;
        ret = db->getData("user",data,"");
        if(ret && !data.isEmpty()){
            qDebug("get ok");
            for(auto d:data){
                qDebug(d["user"].toStdString().c_str());
                qDebug(d["age"].toStdString().c_str());
            }
        }else{
            qDebug("get error");
        }

        //插入数据
        QHash<QString, QString> user;
        user.insert("name","yang");
        user.insert("age","30");
        ret = db->addData("user",user);
        if(ret && db->getNumRowsAffected() != 0){
            qDebug("insert ok");
        }else{
            qDebug("insert error");
        }
    
        //更新
        QHash<QString, QString> update;
        update.insert("age","35");
        ret = db->updateData("user",update,"where age = 30");
        if(ret && db->getNumRowsAffected() != 0){
            qDebug("updateData ok");
        }else{
            qDebug("updateData error");
        }
    
        //删除
        ret = db->delData("user","where age = 32");
        if(ret && db->getNumRowsAffected() != 0){
            qDebug("delete ok");
        }else{
            qDebug("delete error");
        }
    
    }else{
        qDebug("connect error");
    }
}
```
### 有SQL预编译版
```cpp
void MainWindow::on_pb_test_clicked()
{
    MysqlDb* db = new MysqlDb();
    db->setMhost("111.178.126.10");
    db->setMuser("xxxxxx");
    db->setMpwd("xxxxxx");
    bool ret = db->connectSql("test");
    if(ret){
        qDebug("connect ok");
        
        //读取数据(无where)
        QList<QHash<QString, QString>> data;
        ret = db->getDataPS("user",data);
        if(ret && !data.isEmpty()){
            qDebug("get ok");
            for(auto d:data){
                qDebug(d["user"].toStdString().c_str());
                qDebug(d["age"].toStdString().c_str());
            }
        }else{
            qDebug("get error");
        }
        
        //读取数据(有where)
        QList<QHash<QString, QString>> data;
        MysqlDb::SqlWhere sqlWhere;
        sqlWhere.str = "where name = ? and age = ?";
        sqlWhere.values.append("yang");
        sqlWhere.values.append(QString::number(30));
        ret = db->getDataPS("user",data, sqlWhere);
        if(ret && !data.isEmpty()){
            qDebug("get ok");
            for(auto d:data){
                qDebug(d["user"].toStdString().c_str());
                qDebug(d["age"].toStdString().c_str());
            }
        }else{
            qDebug("get error");
        }
        
        //插入数据
        QHash<QString, QString> user;
        user.insert("name","yang");
        user.insert("age","30");
        ret = db->addDataPS("user",user);
        if(ret && db->getNumRowsAffected() != 0){
            qDebug("insert ok");
        }else{
            qDebug("insert error");
        }
        
        //更新
        QHash<QString, QString> update;
        QList<QHash<QString, QString>> data;
        MysqlDb::SqlWhere sqlWhere;
        sqlWhere.str = "where age = ?";
        sqlWhere.values.append(QString::number(30));
        update.insert("age","35");
        ret = db->updateDataPS("user",update,sqlWhere);
        if(ret && db->getNumRowsAffected() != 0){
            qDebug("updateData ok");
        }else{
            qDebug("updateData error");
        }
        
        //删除
        QList<QHash<QString, QString>> data;
        MysqlDb::SqlWhere sqlWhere;
        sqlWhere.str = "where name = ?";
        sqlWhere.values.append("%" + "an" + "%"); // 模糊查询语句，删除名字中带有“an”的用户
        ret = db->delDataPS("user",sqlWhere);
        if(ret && db->getNumRowsAffected() != 0){
            qDebug("delete ok");
        }else{
            qDebug("delete error");
        }
        
    }else{
        qDebug("connect error");
    }   
}
```
注意：`db->getData()`，`db->addData()`，`db->updateData()`，`db->delData()`返回的值仅能代表SQL语句是否执行出错，不能代表返回的值是否为空。

- 若要获取`db->getData()`的读取到的行数，请使用`data.isEmpty()`或使用`db->getNumRowsAffected()`，但不推荐在`db->getData()`时使用后者。

- 若要获取`db->addData()`，`db->addData()`，`db->updateData()`，`db->delData()`的影响到的行数，请使用`db->getNumRowsAffected()`。

- PS版(SQL预编译版)同上

## 附qsqlmysql库的构建

如果已经有该库，以下步骤可忽略。该库位置在Qt5.xx\5.xx\msvc20xx_\plugins\sqldrivers

qsqlmysql库是Qt框架中用于连接MySQL数据库的一个插件库。它是Qt SQL模块的一部分，专门设计用于提供对MySQL数据库的支持。

### 前提条件

1. 安装Qt：确保你已经安装了Qt和Qt Creator。

2. 安装MySQL客户端库： 

- Windows环境下，可以直接下载 MySQL C API 开发包 ()。

- Linux环境，可通过包管理器安装（例如 sudo apt-get install libmysqlclient-dev）。

- macOS环境，可通过Homebrew安装（例如 brew install mysql-client）。下载QT相应版本的源码，比如我的是Qt5.14.2。下载地址：

### 构建步骤

1. 查找Qt的安装路径和编译器

- 确认你的Qt安装路径。例如：C:\Qt\5.x.x\mingwxx_32

- 确认你使用的编译器，如 MinGW 或 MSVC，以及其路径。

- QT的源码不用全解压，只需要qtbase这个文件夹下的全部内容。

2. 设置环境变量

- 将Qt的编译工具（如qmake）添加到系统的PATH变量中。

3. 获取MySQL开发库

- 确保你已经下载并解压了 MySQL Connector/C 用于 Windows 系统；在Linux和macOS系统上安装相应的开发库会自动设置好的路径。

4. 构建 qsqlmysql 插件

- 使用Qtcreator打开D:\Qt\qtbase\src\plugins\sqldrivers\mysql\mysql.pro工程文件。

- 打开工程后，会报错：Cannot read qtsqldrivers-config.pri: No such file or directory

- 接下来需要对mysql.pro文件和它上一级的qsqldriverbase.pri文件做出修改：

- 修改qsqldriverbase.pri

```
QT = core core-private sql-private

#For QMAKE_USE in the parent projects.

#注释到这个

#include($$shadowed($$PWD)/qtsqldrivers-config.pri)

#新增加这个

include(./configure.pri)

PLUGIN_TYPE = sqldrivers

load(qt_plugin)

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
```

- 修改 mysql.pro：

```
TARGET = qsqlmysql

HEADERS += $$PWD/qsql_mysql_p.h

SOURCES += $$PWD/qsql_mysql.cpp $$PWD/main.cpp

#注释到这个

#QMAKE_USE += mysql

OTHER_FILES += mysql.json

PLUGIN_CLASS_NAME = QMYSQLDriverPlugin

#以下为新增

#!!mysql的lib路径

LIBS += D:\Qt\mysql-connector-c-6.1.11-winx64/lib/libmysql.lib

#!!mysql的include路径

INCLUDEPATH += $$quote(D:\Qt\mysql-connector-c-6.1.11-winx64/include)

#!!mysql的include路径

DEPENDPATH += $$quote(D:\Qt\mysql-connector-c-6.1.11-winx64/include)

include(../qsqldriverbase.pri)

#!!设置编译好的qmysql.dll放置的目录

DESTDIR = ../mysql/mysqlDll
```

5. 将编译好的插件拷贝到合适的位置

- 将编译得到的 qsqlmysql.dll（或 libqsqlmysql.so 或 libqsqlmysql.dylib）放到Qt的插件目录下。例如 C:\Qt\5.x.x\mingwxx_64\plugins\sqldrivers（Windows）或 /path/to/qt/plugins/sqldrivers（Linux和macOS）。

### 注意事项

1. 版本匹配：请确保你的 MySQL 客户端库版本与 MySQL 服务器版本兼容，同时确保与 Qt 使用的编译器版本一致。

2. 路径问题：在运行示例程序时，所有路径要使用绝对路径或将相关路径加入到环境变量中以确保 Qt 能够找到相应的库文件。

3. 权限问题：在Linux和macOS环境下，可能需要使用sudo来执行某些命令以拥有足够的权限。

- 按照上述步骤，你应该能够成功构建并使用 qsqlmysql 插件来连接 MySQL 数据库。

- 最后，附上编译好的mysql驱动，含windows和mac版本的（5.14.2，5.15.2，6.5.3）

- 链接如下：
```
https://pan.baidu.com/s/1m15DbFuFTtXfEyqyOS2cew
提取码: 2o2s
```

## 其他资源
- Qt - 操作MySQL数据库
```
https://www.cnblogs.com/zhuchunlin/p/16485933.html
```
- QT连接MYSQL（保姆级成功案例）_qt mysql-CSDN博客
```
https://blog.csdn.net/amateur_dabai/article/details/129820669
```
- QT学习之路——Qt QMySQL driver not loaded问题（笔记）_qsqldatabase: qmysql driver not loaded qsqldatabas-CSDN博客
```
https://blog.csdn.net/qq_39020917/article/details/132223175
```
- MySQL :: Download MySQL Connector/C (Archived Versions)
```
https://downloads.mysql.com/archives/c-c/
```
- QT加载mysql失败，重新构建mysql源文件_qt重新加载资源文件-CSDN博客
```
https://blog.csdn.net/weixin_67622659/article/details/139107946
```
- QT操作Mysql数据库_qt mysql-CSDN博客
```
https://blog.csdn.net/weixin_43473965/article/details/131604894
```
- Qt使用mysql数据库
```
https://www.cnblogs.com/flygreen/p/18029637
```
- Qt连接mysql数据库_不能找到qtsqldrivers-config.pri-CSDN博客
```
https://blog.csdn.net/sunshine0211/article/details/127238842
```
- Linux下Qt 5.15.2源码下载及编译_qt5 linux 源码下载-CSDN博客
```
https://blog.csdn.net/weixin_44596212/article/details/123850740
```
- qt creator mysql_qt creator with mysql-CSDN博客
```
https://blog.csdn.net/wowocpp/article/details/108851808
```
- 编译qt5.15.2（mac/windows）的mysql驱动（附带编译好的文件）_macos编译qt5.15.2-CSDN博客
```
https://blog.csdn.net/qq_33375598/article/details/138327242
```
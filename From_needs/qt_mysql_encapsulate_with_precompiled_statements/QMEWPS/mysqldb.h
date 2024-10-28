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
    bool connectSql(const QString &dbName);//打开连接
    bool connectSql(const QString &host, int port, const QString &dbName, const QString &userName, const QString &password);//打开连接
    bool disConnectSql();//关闭连接
    bool sqlWhereToStr(QList<QHash<QString, QString>> sqlWhere, QList<QString> &values, QString &queryStr);
    void queryBindValue(QSqlQuery &query, QList<QString> values);
    bool queryExec(QString sqlStr);//执行sql语句，不获取结果
    bool queryExec(QString sqlStr,QList<QHash<QString,QString>> &data);//执行sql语句，并获取结果
    bool queryExec(QSqlQuery &query, QString queryStr);
    bool queryExec(QSqlQuery &query, QString queryStr, QList<QHash<QString, QString>> &data);
    bool getData(QString tableName,QHash<QString,QString> &data,QList<QHash<QString, QString>> sqlWhere = QList<QHash<QString, QString>>()); //获取数据
    bool getData(QString table,QList<QHash<QString,QString>> &data,QList<QHash<QString, QString>> sqlWhere = QList<QHash<QString, QString>>()); //获取数据
    bool getData(QString tableName,QHash<QString,QString> columndata,QList<QHash<QString,QString>> &data, QList<QHash<QString, QString>> sqlWhere = QList<QHash<QString, QString>>()); //获取数据
    bool addData(QString tableName,QHash<QString,QString> data);//增加
    bool delData(QString tableName,QList<QHash<QString, QString>> sqlWhere);//删除
    bool updateData(QString tableName,QHash<QString,QString> data,QList<QHash<QString, QString>> sqlWhere = QList<QHash<QString, QString>>());//修改

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

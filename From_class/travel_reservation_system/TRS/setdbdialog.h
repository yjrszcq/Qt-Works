#ifndef SETDBDIALOG_H
#define SETDBDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class SetDbDialog : public QDialog
{
    Q_OBJECT
public:
    SetDbDialog(QString host, int port, QString user, QString password, QString database);

private:
    QLabel *l[5];
    QLineEdit *le[5];
    QPushButton *pb_yes, *pb_no;

signals:
    void dataSent(QString host, int port, QString user, QString password, QString database, bool flag);

public slots:
    void pbYes();
    void pbNo();
};

#endif // SETDBDIALOG_H

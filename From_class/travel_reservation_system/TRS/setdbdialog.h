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
    SetDbDialog(QString config_file_path);

private:
    QLabel *l[5];
    QLineEdit *le[5];
    QPushButton *pb_yes, *pb_no;
    QString config_file_path;

signals:
    void dbSetSent(bool flag);

public slots:
    void pbYes();
    void pbNo();
};

#endif // SETDBDIALOG_H

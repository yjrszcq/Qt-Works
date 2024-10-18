#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "servor.h"

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>


class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(Servor *servor);
    static void show(Servor *servor){
        LoginDialog ld(servor);
        ld.exec();
    }
private:
    QPushButton * pb_login, *pb_signin;
    QLabel *l_option, *l_name, *l_id;
    QLineEdit *le_name, *le_id;
    QRadioButton *rb_user, *rb_root;

public slots:
    void pbLogin();
    void nameHide();
    void nameShow();

private:
    Servor *servor;
};

#endif // LOGINDIALOG_H

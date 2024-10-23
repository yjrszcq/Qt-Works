#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "server.h"

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>


class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(Server *server);
    static void show(Server *server){
        LoginDialog ld(server);
        ld.exec();
    }
private:
    QPushButton * pb_login;
    QLabel *l_option, *l_name, *l_password;
    QLineEdit *le_name, *le_password;
    QRadioButton *rb_user, *rb_root;

public slots:
    void pbLogin();
    void nameHide();
    void nameShow();

private:
    Server *server;
};

#endif // LOGINDIALOG_H

#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <server.h>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class SignupDialog : public QDialog
{
    Q_OBJECT
public:
    SignupDialog(Server *server);
    static void show(Server *server){
        SignupDialog sd(server);
        sd.exec();
    }

private:
    Server *server;
    QLabel *l_name, *l_id, *l_password;
    QLineEdit *le_name, *le_id, *le_password;
    QPushButton *pb_signup;

public slots:
    void pbSignup();
};

#endif // SIGNUPDIALOG_H

#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <servor.h>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class SignupDialog : public QDialog
{
    Q_OBJECT
public:
    SignupDialog(Servor *servor);
    static void show(Servor *servor){
        SignupDialog sd(servor);
        sd.exec();
    }

private:
    Servor *servor;
    QLabel *l_name, *l_id;
    QLineEdit *le_name, *le_id;
    QPushButton *pb_signup;

public slots:
    void pbSignup();
};

#endif // SIGNUPDIALOG_H

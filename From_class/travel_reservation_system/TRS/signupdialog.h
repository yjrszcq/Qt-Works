#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <servor.h>
#include <QDialog>

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
};

#endif // SIGNUPDIALOG_H

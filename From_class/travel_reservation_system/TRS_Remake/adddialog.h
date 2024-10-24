#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include "server.h"

#include <QDialog>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(Server *server, int flag, QWidget *parent = nullptr);
    static void show(Server *server, int flag){
        AddDialog ad(server ,flag);
        ad.exec();
    }
    ~AddDialog();

private slots:
    void on_pb_add_clicked();

    void on_pb_cancel_clicked();

    void on_cb_resv_1_currentIndexChanged(int index);

private:
    Server *server;
    int flag;

private:
    Ui::AddDialog *ui;
};

#endif // ADDDIALOG_H

#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "passwordinfo.h"

class HistoryDialog : public QDialog
{
    Q_OBJECT
public:
    HistoryDialog(QString);
    static void show(QString password){
        HistoryDialog history(password);
        history.exec();
    }
public slots:
    void set_info(int index);
    void save_info();
    void delete_info();
    void load_data();
    void save_data(int jud);
private:
    std::string transform(std::string);
    QComboBox *cb_choice;
    QStackedWidget *sw_info;
    QLineEdit *le_website;
    QLineEdit *le_username;
    QLineEdit *le_password;
    QPushButton *pb_save, *pb_delete;
    std::vector<PasswordInfo> infos;
    QString password;
};

#endif // HISTORYDIALOG_H

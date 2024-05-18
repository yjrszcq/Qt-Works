#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "passwordinfo.h"

class HistoryDialog : public QDialog
{
    Q_OBJECT
public:
    HistoryDialog(QString password);
    static void show(QString password){
        HistoryDialog history(password);
        history.exec();
    }
public slots:
    void setInfo(int index);
    void saveInfo();
    void deleteInfo();
    void loadData();
    void saveData(int jud);
private:
    void saveMessage(int jud);
    QComboBox *cb_choice;
    QLineEdit *le_website;
    QLineEdit *le_username;
    QLineEdit *le_password;
    QPushButton *pb_save, *pb_delete;
    std::vector<PasswordInfo> infos;
    QString password;
};

#endif // HISTORYDIALOG_H

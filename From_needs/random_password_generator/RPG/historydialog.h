#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

class HistoryDialog : public QDialog
{
    Q_OBJECT
public:
    typedef struct Info{
        QString website;
        QString username;
        QString password;
    }Info;
    HistoryDialog(QString, std::vector<HistoryDialog::Info>);
    static void show(QString password, std::vector<HistoryDialog::Info> infos){
        HistoryDialog history(password, infos);
        history.exec();
    }
public slots:
    void set_info(int index);
    void save_info();
    void delete_info();
    void save_data();
private:
    QComboBox *cb_choice;
    QStackedWidget *sw_info;
    QLineEdit *le_website;
    QLineEdit *le_username;
    QLineEdit *le_password;
    QPushButton *pb_save, *pb_delete;
    std::vector<Info> infos;
    QString password;
};

#endif // HISTORYDIALOG_H

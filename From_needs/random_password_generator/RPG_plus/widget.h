#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void setLength();
    void setMark(int jud);
    void generatePassword();
    void generateHistory();

private:
    int length;
    std::string library;
    QLineEdit *le_length;
    QLineEdit *le_password;
    QComboBox *cb_mark;
    QPushButton *pb_confirm, *pb_history;
};
#endif // WIDGET_H

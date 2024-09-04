#ifndef KEY_H
#define KEY_H

#include <QWidget>
#include <QPushButton>

class Key : public QWidget
{
    Q_OBJECT
public:
    Key(QString, QString, int, int, bool, QPoint, QWidget *parent = nullptr);
    ~Key();

public:
    QString keyword;
    QString name;

private:
    QPushButton* pb;

private slots:
    void simulateKeyPress();
};

#endif // KEY_H

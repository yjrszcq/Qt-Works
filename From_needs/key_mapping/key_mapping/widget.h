#ifndef WIDGET_H
#define WIDGET_H

#include <bits/stdc++.h>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>

#include <QTimer>
#include <QLabel>
#include <QPoint>
#include <QCursor>
#include <QMouseEvent>

#include "key.h"
#include "clickboard.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void onDataReceived(const QPoint&);
    void addOrDelete();

private:
    std::vector<Key*> keys;
    QString adr;
    QPushButton* pb_confirm;
    QRadioButton* rb_add;
    QRadioButton* rb_delete;
    QCheckBox* cb_top;
    QLineEdit* le_name;
    QLineEdit* le_key;
    QSpinBox* sb_size;
    QSpinBox* sb_transparency;
};

#endif // WIDGET_H

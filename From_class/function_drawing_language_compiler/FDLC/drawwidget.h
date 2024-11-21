#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode{DIRECT, FILE};
    DrawWidget();
    void paintEvent(QPaintEvent *event) override;
};

#endif // DRAWWIDGET_H

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
    int min_x = std::numeric_limits<int>::max(), min_y = std::numeric_limits<int>::max(), max_x = 0, max_y = 0;
};

#endif // DRAWWIDGET_H

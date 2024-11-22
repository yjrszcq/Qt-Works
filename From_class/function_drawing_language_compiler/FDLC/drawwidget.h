#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QUrl>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode{DIRECT, FILE};
    DrawWidget();
    void paintEvent(QPaintEvent *event) override;
    static void outputPixmap(QUrl file_path, QString format);
};

#endif // DRAWWIDGET_H

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QUrl>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode{SINGLE, MULTIPLE};
    DrawWidget();
    void paintEvent(QPaintEvent *event) override;
    static void countSize(int &x, int &y);
    static void outputPixmap(QList<QUrl> file_path, QString format, Mode mode);
};

#endif // DRAWWIDGET_H

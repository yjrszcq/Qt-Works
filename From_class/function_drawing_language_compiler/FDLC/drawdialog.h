#ifndef DRAWDIALOG_H
#define DRAWDIALOG_H

#include <QDialog>

class DrawDialog : public QDialog
{
    Q_OBJECT
public:
    enum Mode{DIRECT, FILE};
    DrawDialog();
    void paintEvent(QPaintEvent *event) override;
};

#endif // DRAWDIALOG_H

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include "parsers.h"

#include <QPainter>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawWidget(Parsers &p, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawTotalXY(QPainter &painter, Parsers &parser, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y);
    void drawXY(QPainter &painter, double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b);
    void outTextXY(QPainter &painter, double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b);

private:
    Parsers &parser;
};

#endif // DRAWWIDGET_H

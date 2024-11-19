#include "drawwidget.h"

DrawWidget::DrawWidget(Parsers &p, QWidget *parent)
    : QWidget(parent), parser(p)
{}

void DrawWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int i = 0; i < parser.start_values.size(); ++i) {
        drawTotalXY(painter, parser, parser.origin_x_values[i], parser.origin_y_values[i], parser.scale_x_values[i],
            parser.scale_y_values[i], parser.rot_ang_values[i], parser.r_values[i], parser.g_values[i],
            parser.b_values[i], parser.start_values[i], parser.end_values[i], parser.step_values[i],
            parser.for_x_values[i], parser.for_y_values[i]
        );
    }

    for (int i = 0; i < parser.notes_string_values.size(); ++i) {
        outTextXY(painter, parser.notes_x_values[i], parser.notes_y_values[i], parser.notes_string_values[i],
            parser.notes_r_values[i], parser.notes_g_values[i], parser.notes_b_values[i]
        );
    }
}

void DrawWidget::drawTotalXY(QPainter &painter, Parsers &parser, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y) {
    double x, y;
    double parameter = start;
    if (step > 0) {
        while (parameter <= end) {
            x = parser.getExpValue(for_x);
            y = parser.getExpValue(for_y);
            drawXY(painter, x, y, origin_x, origin_y, scale_x, scale_y, rot_ang, r, g, b);
            parameter += step;
        }
    } else if (step < 0) {
        while (parameter >= end) {
            x = parser.getExpValue(for_x);
            y = parser.getExpValue(for_y);
            drawXY(painter, x, y, origin_x, origin_y, scale_x, scale_y, rot_ang, r, g, b);
            parameter += step;
        }
    }
}

void DrawWidget::drawXY(QPainter &painter, double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b) {
    double temp_x, temp_y;
    x *= scale_x;
    y *= scale_y;
    temp_x = x;
    temp_y = y;
    x = temp_x * cos(rot_ang) + temp_y * sin(rot_ang);
    y = temp_y * cos(rot_ang) - temp_x * sin(rot_ang);
    x += origin_x;
    y += origin_y;
    painter.setPen(QColor(r, g, b));
    painter.drawPoint(x, y);
}

void DrawWidget::outTextXY(QPainter &painter, double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b) {
    QColor color(round(notes_r), round(notes_g), round(notes_b));
    painter.setPen(color);
    painter.drawText(notes_x, notes_y, notes_string);
}

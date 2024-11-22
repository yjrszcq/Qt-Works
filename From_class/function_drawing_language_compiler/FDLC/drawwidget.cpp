#include "drawwidget.h"

#include <QPainter>
#include "globaldefines.h"

DrawWidget::DrawWidget() {
    this->setWindowTitle("图像");
}

void DrawWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (int i = 0; i < draw_node.size(); ++i) {
        QPen pen(QColor(draw_node[i].r, draw_node[i].g, draw_node[i].b));
        pen.setWidth(draw_node[i].pix);
        painter.setPen(pen);
        painter.drawPoint(draw_node[i].x, draw_node[i].y);
    }
    for (int i = 0; i < draw_text.size(); ++i) {
        QPen pen(QColor(round(draw_text[i].set.r), round(draw_text[i].set.g), round(draw_text[i].set.b)));
        pen.setWidth(draw_text[i].set.pix);
        painter.setPen(pen);
        painter.drawText(draw_text[i].set.x, draw_text[i].set.y, draw_text[i].str);
    }
}

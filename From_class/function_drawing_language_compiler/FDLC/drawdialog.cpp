#include "drawdialog.h"
#include "globaldefines.h"

#include <QPainter>

DrawDialog::DrawDialog() {
    this->setWindowTitle("图像");
}

void DrawDialog::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (int i = 0; i < draw_node.size(); ++i) {
        painter.setPen(QColor(draw_node[i].r, draw_node[i].g, draw_node[i].b));
        painter.drawPoint(draw_node[i].x, draw_node[i].y);
    }
    for (int i = 0; i < draw_text.size(); ++i) {
        QColor color(round(draw_text[i].set.r), round(draw_text[i].set.g), round(draw_text[i].set.b));
        painter.setPen(color);
        painter.drawText(draw_text[i].set.x, draw_text[i].set.y, draw_text[i].str);
    }
}

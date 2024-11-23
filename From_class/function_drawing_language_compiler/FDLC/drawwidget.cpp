#include "drawwidget.h"
#include "globaldefines.h"

#include <QPainter>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>


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

void DrawWidget::countSize(int &x, int &y){
    int min_x = x, min_y = y, max_x = 0, max_y = 0;
    for(int i = 0; i < draw_node.size(); ++i){
        if(draw_node[i].x < min_x) {min_x = draw_node[i].x;}
        if(draw_node[i].x > max_x) {max_x = draw_node[i].x;}
        if(draw_node[i].y < min_y) {min_y = draw_node[i].y;}
        if(draw_node[i].y > max_y) {max_y = draw_node[i].y;}
    }
    for (int i = 0; i < draw_text.size(); ++i){
        if(draw_text[i].set.x < min_x) {min_x = draw_text[i].set.x;}
        if(draw_text[i].set.x > max_x) {max_x = draw_text[i].set.x;}
        if(draw_text[i].set.y < min_y) {min_y = draw_text[i].set.y;}
        if(draw_text[i].set.y > max_y) {max_y = draw_text[i].set.y;}
    }
    x = max_x + min_x;
    y = max_y + min_y;
}


void DrawWidget::outputPixmap(QList<QUrl> file_path, QString format, Mode mode){
    try{
        if(format.length() > 4){
            QString error = "格式错误";
            throw std::runtime_error((error).toStdString());
        }
        int min_x = draw_node[0].x, min_y = draw_node[0].y, max_x = 0, max_y = 0;
        QImage image(10000, 10000, QImage::Format_ARGB32);
        if(format == "PNG"){
            image.fill(Qt::transparent);
        } else{
            image.fill(Qt::white);
        }
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);

        for (int i = 0; i < draw_node.size(); ++i) {
            QPen pen(QColor(draw_node[i].r, draw_node[i].g, draw_node[i].b));
            pen.setWidth(draw_node[i].pix);
            painter.setPen(pen);
            painter.drawPoint(draw_node[i].x, draw_node[i].y);
            if(draw_node[i].x < min_x) {min_x = draw_node[i].x;}
            if(draw_node[i].x > max_x) {max_x = draw_node[i].x;}
            if(draw_node[i].y < min_y) {min_y = draw_node[i].y;}
            if(draw_node[i].y > max_y) {max_y = draw_node[i].y;}

        }

        for (int i = 0; i < draw_text.size(); ++i) {
            QPen pen(QColor(round(draw_text[i].set.r), round(draw_text[i].set.g), round(draw_text[i].set.b)));
            pen.setWidth(draw_text[i].set.pix);
            painter.setPen(pen);
            painter.drawText(draw_text[i].set.x, draw_text[i].set.y, draw_text[i].str);
            if(draw_text[i].set.x < min_x) {min_x = draw_text[i].set.x;}
            if(draw_text[i].set.x > max_x) {max_x = draw_text[i].set.x;}
            if(draw_text[i].set.y < min_y) {min_y = draw_text[i].set.y;}
            if(draw_text[i].set.y > max_y) {max_y = draw_text[i].set.y;}
        }

        painter.end();

        QRect boundingRect(0, 0, max_x + min_x, max_y + min_y);

        QPixmap pixmap(boundingRect.size());

        if(format == "PNG"){
            pixmap.fill(Qt::transparent);
        } else{
            pixmap.fill(Qt::white);
        }

        QPainter finalPainter(&pixmap);
        finalPainter.drawImage(pixmap.rect(), image, boundingRect);
        finalPainter.end();

        char ch[4];
        for(int i = 0; i < format.length(); i ++){
            ch[i] = format[i].toLatin1();
        }
        QFileInfo fileInfo(file_path[0].url());
        QString baseName = fileInfo.baseName();
        QString default_file_path = fileInfo.absolutePath() + "/" + baseName + "." + format.toLower();
        switch(mode){
        case SINGLE:{
            if(!pixmap.save(default_file_path, ch)){
                QString error = "无法导出文件";
                throw std::runtime_error((error).toStdString());
            }
            break;
        }
        case MULTIPLE:{
            QString filter =  format + " Files (*." + format.toLower() + ")";
            QString path = QFileDialog::getSaveFileName(nullptr, "保存", default_file_path, filter);
            if(!pixmap.save(path, ch)){
                QString error = "无法导出文件";
                throw std::runtime_error((error).toStdString());
            }
            break;
        }
        }
        QMessageBox::information(nullptr, "成功", "图像导出成功", QMessageBox::Yes);
    } catch(const std::exception &e){
        throw std::runtime_error(std::string(e.what()));
    }
}

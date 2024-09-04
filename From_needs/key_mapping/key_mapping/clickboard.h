#ifndef CLICKBOARD_H
#define CLICKBOARD_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPoint>
#include <QCursor>
#include <QMouseEvent>

class ClickBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ClickBoard(QWidget *parent = nullptr);
    static QPoint getPoint(){
        ClickBoard* click = new ClickBoard();
        QPoint pos;
        if(click->capturing == false){
            pos = click->globalPos;
        }
    }
private:
    bool capturing;
    QPoint globalPos;
    QTimer *captureTimer;
    QLabel *lb_position;
    void startCapture();
    void updateMousePosition();
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            // 捕获鼠标位置并显示
            globalPos = event->globalPos();
            lb_position->setText(QString("结束位置：(%1, %2)").arg(globalPos.x()).arg(globalPos.y()));
            // 结束捕获
            capturing = false;
            captureTimer->stop();
            emit dataSent(globalPos);  // 发送信号，将数据传回给主窗口
            this->close();  // 关闭子窗口
        } else{
            this->close();
        }
    }
signals:
    void dataSent(const QPoint &data);
    void dataSentTiming(const QPoint &data);
};

#endif // CLICKBOARD_H

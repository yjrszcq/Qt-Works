#include "clickboard.h"
#include <QVBoxLayout>

ClickBoard::ClickBoard(QWidget *parent)
    : QWidget{parent}
{
    // 设置窗口为无边框并透明背景
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowOpacity(0.5);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->showFullScreen();
    lb_position = new QLabel();
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(lb_position);
    this->setLayout(vbox);
    startCapture();
}


void ClickBoard::startCapture() {
    // 开始捕获，设置标志位
    capturing = true;
    lb_position->setText("捕获中...");

    // 设置定时器，实时更新鼠标位置
    captureTimer = new QTimer(this);
    connect(captureTimer, &QTimer::timeout, this, &ClickBoard::updateMousePosition);
    captureTimer->start(100); // 每100ms更新一次
}


void ClickBoard::updateMousePosition() {
    if (capturing) {
        QPoint globalPos = QCursor::pos();
        lb_position->setText(QString("当前位置: (%1, %2)").arg(globalPos.x()).arg(globalPos.y()));
    }
}

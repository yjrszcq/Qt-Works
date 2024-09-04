#include "key.h"
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QApplication>

Key::Key(QString keyword, QString name, int size, int transparency, bool top, QPoint pos, QWidget *parent)
    : QWidget{parent}
{
    this->keyword = keyword;
    this->name = name;
    this->move(pos);
    // 设置窗口为无边框并透明背景
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 初始状态：窗口置顶
    if(top == true){
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    // 窗口在创建时不会获得焦点
    // this->setWindowFlags(this->windowFlags() | Qt::WindowDoesNotAcceptFocus);

    this->show();

    this->setFixedSize(size * 20, size * 20);

    pb = new QPushButton(name);
    pb->setFixedSize(this->size());
    // 设置按钮的透明度
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(pb);
    opacityEffect->setOpacity((double)transparency / 10.0);
    pb->setGraphicsEffect(opacityEffect);

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(pb);

    this->setLayout(vbox);

    connect(pb, SIGNAL(clicked(bool)), this, SLOT(simulateKeyPress()));
}

Key::~Key(){}

void Key::simulateKeyPress() {
    // 创建一个按下 'A' 键的事件
    // 将事件发送给当前窗口

    if(keyword == "A"){
        QWidget *receiver = QApplication::focusWidget();
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_A, Qt::NoModifier);
        QApplication::sendEvent(receiver, &keyPress);
        QApplication::sendEvent(receiver, &keyRelease);
    }

}

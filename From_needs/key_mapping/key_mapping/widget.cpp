#include "widget.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    le_key = new QLineEdit();
    le_name = new QLineEdit();

    sb_size = new QSpinBox();
    sb_transparency = new QSpinBox();

    cb_top = new QCheckBox("位于最上层");

    rb_add = new QRadioButton("添加按键");
    rb_delete = new QRadioButton("删除按键");

    pb_confirm = new QPushButton("确认");

    QFormLayout* form = new QFormLayout();
    form->addRow("按键", le_key);
    form->addRow("命名", le_name);
    form->addRow("尺寸", sb_size);
    form->addRow("透明度", sb_transparency);


    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(rb_add);
    hbox->addWidget(rb_delete);

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addLayout(form);
    vbox->addWidget(cb_top);
    vbox->addLayout(hbox);
    vbox->addWidget(pb_confirm);

    this->setLayout(vbox);

    sb_size->setMaximum(10);
    sb_size->setMinimum(1);
    sb_size->setValue(5);
    sb_transparency->setMaximum(10);
    sb_transparency->setMinimum(1);
    sb_transparency->setValue(5);

    cb_top->setChecked(true);
    rb_add->setChecked(true);

    connect(le_key, &QLineEdit::textChanged, le_name, &QLineEdit::setText);
    connect(pb_confirm, &QPushButton::clicked, this, &Widget::addOrDelete);

}

Widget::~Widget() {}

void Widget::addOrDelete() {
    if(rb_add->isChecked()){
        ClickBoard* click = new ClickBoard();
        connect(click, &ClickBoard::dataSent, this, &Widget::onDataReceived);
    } else{
        for(int i = 0; i < keys.size(); i ++){
            if(keys[i]->name == le_name->text()){
                keys[i]->close();
                keys.erase(keys.begin() + i);
                break;
            }
        }
    }
}

void Widget::onDataReceived(const QPoint &position) {
    // 处理接收到的数据
    keys.push_back(new Key(le_key->text(), le_name->text(), sb_size->value(), sb_transparency->value(), cb_top->isChecked(), position));
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(!connectToServor()){
        QMessageBox::critical(this , "错误", "Servor连接失败", QMessageBox::Yes);
        QCoreApplication::exit(2);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    delete servor;
}

void MainWindow::exitReceive(int flag){
    QCoreApplication::exit(flag);
}

bool MainWindow::connectToServor(){
    servor = NULL;
    servor = new Servor();
    connect(servor, SIGNAL(exitSent(int)), this, SLOT(exitReceive(int)));
    if(servor != NULL){
        return true;
    } else{
        return false;
    }
}


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
        QApplication::exit(0);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    delete servor;
}

bool MainWindow::connectToServor(){
    servor = NULL;
    servor = new Servor();
    if(servor != NULL){
        return true;
    } else{
        return false;
    }
}


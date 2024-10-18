#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectToServor();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete servor;
}

bool MainWindow::connectToServor(){
    servor = new Servor();
}


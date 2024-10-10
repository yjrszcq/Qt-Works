#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::open_file(){
    file_url = QFileDialog::getOpenFileUrl();
    qDebug() << file_url;


    // 如果文件打开成功
    ui->label_file_name->setText(file_url.fileName());
    text_changed_flag = false;
    if(!ui->textEdit_text->document()->isEmpty()){
        ui->label_flie_state_display->setText("已保存");
    } else{
        ui->label_flie_state_display->setText("未保存");
    }

    return true;
}


void MainWindow::on_action_open_triggered()
{
    if(!text_changed_flag){
        open_file();
    } else{
        QMessageBox* msg = new QMessageBox(QMessageBox::Warning, "警告", "文件未保存，是否继续？", QMessageBox::Yes | QMessageBox::No, this);
        int iResult = msg->exec();
        switch(iResult){
        case QMessageBox::Yes: open_file(); break;
            default : break;
        }
    }
}


void MainWindow::on_action_color_triggered()
{
    QColor color =QColorDialog::getColor();
    ui->textEdit_text->setTextColor(color);
}


void MainWindow::on_action_font_triggered()
{
    bool ok;
    QFont myfont = QFontDialog::getFont(&ok);
    if(ok){
        ui->textEdit_text->setFont(myfont);
    }
}


void MainWindow::on_textEdit_text_textChanged()
{
    text_changed_flag = true;
    ui->label_flie_state_display->setText("未保存");
    qDebug() << "text_changed";
}


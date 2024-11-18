#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ce = new CodeEditor(this);
    ui->setupUi(this);

    QFont font("Consolas", 11);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setWeight(QFont::Normal);
    ce->setFont(font);
    ui->vl_text->insertWidget(1, ce);

    connect(ce, &QPlainTextEdit::textChanged, this, [=](){
        if(jud_status_change == true){
            ui->l_text_file_status_display->setText("未保存");
        }
        jud_status_change = true;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::readFile(QUrl file_path){
    QString content;
    try {
        QFile file(file_path.url());
        if (!file.exists()) {
            throw std::runtime_error("文件不存在");
        }

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("无法打开文件: " + file.errorString().toStdString());
        }
        QTextStream in(&file);
        in.setAutoDetectUnicode(true);
        try {
            content = in.readAll();
        } catch (const std::exception& e) {
            file.close();
            throw std::runtime_error("读取文件内容时发生错误: " + std::string(e.what()));
        }
        file.close();
    } catch (const std::exception& e) {
        qDebug() << "错误:" << e.what();
        throw std::runtime_error(std::string(e.what()));
    }
    return content;
}

void MainWindow::saveFile(QUrl file_path){
    try {
        QFile file(file_path.url());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
            throw std::runtime_error("无法打开文件: " + file.errorString().toStdString());
        }
        QTextStream out(&file);
        try {
            out << ce->toPlainText();
        } catch (const std::exception &e) {
            file.close();
            throw std::runtime_error("写入文件内容时发生错误: " + std::string(e.what()));
        }
        file.close();
    } catch(const std::exception &e){
        qDebug() << "错误:" << e.what();
        throw std::runtime_error(std::string(e.what()));
    }
}

void MainWindow::on_new_file_triggered()
{
    MainWindow *newWindow = new MainWindow();
    newWindow->show();
}


void MainWindow::on_open_file_triggered()
{
    QString defaultFilePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator();
    QString filter =  "All Files (*);;Text Files (*.txt)";
    file_path = QFileDialog::getOpenFileName(nullptr, "打开", defaultFilePath, filter);
    if(file_path.url() != ""){
        QString text;
        try{
            text = readFile(file_path);
        } catch (const std::exception& e){
            QMessageBox::critical(nullptr, "错误", QString::fromStdString(e.what()), QMessageBox::Yes);
            return;
        }
        ce->setPlainText(text);
        ui->l_text_current_file_display->setText(file_path.fileName());
        ui->l_text_file_status_display->setText("已保存");
        jud_status_change = false;
    } else{
        file_path.clear();
    }
}


void MainWindow::on_save_flie_triggered()
{
    if(!file_path.isValid() || file_path.url() == ""){
        QString defaultFileName = "新文本文件.txt";
        QString defaultFilePath = QDir::currentPath() + QDir::separator() + defaultFileName;
        QString filter =  "All Files (*);;Text Files (*.txt)";
        file_path = QFileDialog::getSaveFileName(nullptr, "保存", defaultFilePath, filter);
        if(file_path.url() == ""){
            file_path.clear();
            return;
        }
    }
    try{
        saveFile(file_path);
    } catch(const std::exception& e){
        QMessageBox::critical(nullptr, "失败", QString::fromStdString(e.what()), QMessageBox::Yes);
        return;
    }
    ui->l_text_file_status_display->setText("已保存");
    jud_status_change = true;
}


void MainWindow::on_font_triggered()
{
    bool ok;
    QFont myfont = QFontDialog::getFont(&ok, ce->font());
    if(ok){
        ce->setFont(myfont);
    }
}


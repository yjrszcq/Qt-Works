#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scanner.h"
#include "parsers.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QSplitter>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ce = new CodeEditor(this);
    te_result = new QTextEdit(this);

    QSplitter *splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(ce);
    splitter->addWidget(te_result);

    l_file_status = new QLabel("状态: 未保存");
    cb_auto_output_nodes =new QCheckBox("自动导出点文件");

    ui->statusbar->addWidget(l_file_status);
    ui->statusbar->addPermanentWidget(cb_auto_output_nodes);

    ui->vl_text->insertWidget(1, splitter);

    QFont font("Consolas", 11);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setWeight(QFont::Normal);
    ce->setFont(font);
    te_result->hide();
    te_result->setReadOnly(true);

    connect(ce, &QPlainTextEdit::textChanged, this, [=](){
        if(jud_status_change == true){
            l_file_status->setText("状态: 未保存");
        }
        jud_status_change = true;
    });

    setCRAvail(false);
    setOutputAvail(false);
    status = UNCOMPILED;
    ui->a_output_nodes->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearReceive(){
    te_result->clear();
}

void MainWindow::processReceive(QString process, Qt::GlobalColor color){
    te_result->append(process);
    changeLastLineColor(color);
}

void MainWindow::resultReceive(){
    status = COMPILED;
    ui->a_output_nodes->setEnabled(true);
}

void MainWindow::errorReceive(QString error, Compiler::Status c_status, Scanner::Status s_status, Parsers::Status p_status){
    QString compiler_sataus;
    QString scanner_status;
    QString parsers_status;
    switch(c_status){
    case Compiler::C_PREPARING: compiler_sataus = "PREPARING"; break;
    case Compiler::C_START: compiler_sataus = "START"; break;
    case Compiler::C_SCANNING: compiler_sataus = "SCANNING"; break;
    case Compiler::C_PARSERING: compiler_sataus = "PARSERING"; break;
    case Compiler::C_SUCCEED: compiler_sataus = "SUCCEED"; break;
    case Compiler::C_FAILED: compiler_sataus = "FAILED"; break;
    }
    switch(s_status){
    case Scanner::S_PREPARING: scanner_status = "PREPARING"; break;
    case Scanner::S_START: scanner_status = "START"; break;
    case Scanner::S_SUCCEED:  scanner_status = "SUCCEED"; break;
    }
    switch(p_status){
    case Parsers::P_PREPARING: parsers_status = "PREPARING"; break;
    case Parsers::P_START: parsers_status = "START"; break;
    case Parsers::P_NOTES: parsers_status = "NOTES"; break;
    case Parsers::P_COLOR: parsers_status = "COLOR"; break;
    case Parsers::P_PIXSIZE: parsers_status = "PIXSIZE"; break;
    case Parsers::P_ORIGIN: parsers_status = "ORIGIN"; break;
    case Parsers::P_ROT: parsers_status = "ROT"; break;
    case Parsers::P_SCALE: parsers_status = "SCALE"; break;
    case Parsers::P_FOR: parsers_status = "FOR"; break;
    case Parsers::P_OUTPUT: parsers_status = "OUTPUT"; break;
    case Parsers::P_SUCCEED: parsers_status = "SUCCEED"; break;
    }
    QString error_string = "ERROR: " + error;
    QString status_string = "STATUS: <COMPILER-" + compiler_sataus + "> <SCANNER-" + scanner_status + "> <PARSERS-" + parsers_status + ">";
    QList<QString> err = error_string.split("\n");
    for(auto &e : err){
        if(e != ""){
            te_result->append(e);
            changeLastLineColor(Qt::red);
        }
    }
    te_result->append(status_string);
    changeLastLineColor(Qt::red);
    status = UNCOMPILED;
    ui->a_output_nodes->setEnabled(false);
}

void MainWindow::setCRAvail(bool jud){
    ui->a_compile->setEnabled(jud);
    ui->a_compile_and_run->setEnabled(jud);
    ui->a_color->setEnabled(!jud);
}

void MainWindow::setCRAvail(QString suffix){
    if(suffix != "d"){
        setCRAvail(false);
    } else{
        setCRAvail(true);
    }
}

void MainWindow::setOutputAvail(bool jud){
    ui->m_output_picture->setEnabled(jud);
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

void MainWindow::saveFile(QUrl file_path, QString data){
    try {
        QFile file(file_path.url());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
            throw std::runtime_error("无法打开文件: " + file.errorString().toStdString());
        }
        QTextStream out(&file);
        try {
            out << data;
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

bool MainWindow::readyToReadFile(FileType jud){
    initAll();
    switch(jud){
    case TEXT:{
        QString default_file_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator();
        QString filter =  "All Files (*);;Text Files (*.txt);;Draw Code Files (*.d)";
        file_path = QFileDialog::getOpenFileName(nullptr, "打开", default_file_path, filter);
        if(file_path.url() != ""){
            QString text;
            try{
                text = readFile(file_path);
            } catch (const std::exception& e){
                QMessageBox::critical(nullptr, "错误", QString::fromStdString(e.what()), QMessageBox::Yes);
                return false;
            }
            ce->setPlainText(text);
            ui->l_text_current_file_display->setText(file_path.fileName());
            l_file_status->setText("状态: 已保存");
            jud_status_change = false;
            te_result->hide();
            setCRAvail(QFileInfo(file_path.url()).suffix());
            setOutputAvail(false);
            status = UNCOMPILED;
            ui->a_output_nodes->setEnabled(false);
            return true;
        } else{
            file_path.clear();
            return false;
        }
        break;
    }
    case NODE: case NODE_THIS:{
        QString default_file_path;
        QString filter;
        QStringList file_paths;
        temp_path.clear();
        if(jud == NODE){
            default_file_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator();
            filter =  "Draw Node Files (*.dn);;Draw Text Files (*.dt)";
            file_paths = QFileDialog::getOpenFileNames(nullptr, "打开多个文件", default_file_path, filter);
            for(auto &fp : file_paths){
                temp_path.append(QUrl(fp));
            }
        } else {
            QFileInfo file_info(file_path.url());
            default_file_path = file_info.path() + "/" + file_info.baseName();
            if(QFile::exists(default_file_path + ".dn")){
                file_paths.append(default_file_path + ".dn");
            }
            if(QFile::exists(default_file_path + ".dt")){
                file_paths.append(default_file_path + ".dt");
            }
            if(file_paths.size() == 0){
                QMessageBox::critical(nullptr, "失败", "请先编译", QMessageBox::Yes);
                return false;
            }
        }
        if(!file_paths.isEmpty()){
            for(auto &fp : file_paths){
                try{
                    qDebug() << QFileInfo(fp).suffix();
                    if(QFileInfo(fp).suffix() == "dn"){
                        QString nodes = readFile(QUrl(fp));
                        if(!readNodes(nodes)){
                            QMessageBox::warning(nullptr, "警告", "有文件读取失败", QMessageBox::Yes);
                        }
                    } else if(QFileInfo(fp).suffix() == "dt"){
                        QString texts = readFile(QUrl(fp));
                        if(!readTexts(texts)){
                            QMessageBox::warning(nullptr, "警告", "有文件读取失败", QMessageBox::Yes);
                        }
                    } else {
                        QMessageBox::critical(nullptr, "错误", "文件类型错误, 请重新选择文件", QMessageBox::Yes);
                        return false;
                    }
                } catch(const std::exception& e){
                    QMessageBox::warning(nullptr, "有文件打开失败", QString::fromStdString(e.what()), QMessageBox::Yes);
                }
            }
        } else {
            return false;
        }
        break;
    }
    case CODE: case AS: QMessageBox::warning(nullptr, "错误", "准备读取文件时错误", QMessageBox::Yes); return false;
    }
    return true;
}

bool MainWindow::readyToSaveFile(QString data, FileType jud){
    if(!file_path.isValid() || file_path.url() == "" || jud == AS){
        QString defaultFileName, default_file_path, filter;
        switch(jud){
        case TEXT:{
            defaultFileName = "新文本文件.txt";
            filter =  "All Files (*);;Text Files (*.txt);;Draw Code Files (*.d)";
            default_file_path = QDir::currentPath() + QDir::separator() + defaultFileName;
            break;
        }
        case CODE:{
            defaultFileName = "new codes.d";
            filter =  "Draw Code Files (*.d)";
            default_file_path = QDir::currentPath() + QDir::separator() + defaultFileName;
            break;
        }
        case AS:{
            filter =  "All Files (*);;Text Files (*.txt);;Draw Code Files (*.d)";
            if(!file_path.isValid() || file_path.url() == ""){
                default_file_path = QDir::currentPath() + QDir::separator() + defaultFileName;
            } else{
                default_file_path = file_path.url();
            }
            break;
        }
        case NODE: case NODE_THIS: QMessageBox::critical(nullptr, "错误", "准备保存文件时错误", QMessageBox::Yes); return false;
        }
        file_path = QFileDialog::getSaveFileName(nullptr, "保存", default_file_path, filter);
        if(file_path.url() == ""){
            file_path.clear();
            return false;
        }
    }
    try{
        saveFile(file_path, data);
    } catch(const std::exception& e){
        QMessageBox::critical(nullptr, "失败", QString::fromStdString(e.what()), QMessageBox::Yes);
        return false;
    }
    l_file_status->setText("状态: 已保存");
    ui->l_text_current_file_display->setText(file_path.fileName());
    jud_status_change = true;
    temp_path.clear();
    return true;
}

bool MainWindow::readNodes(QString data){
    QStringList lines = data.split("\n");
    QVector<struct DrawNode> temp;
    for(auto &l : lines){
        QStringList nodes = l.split(" ");
        if(nodes.size() == 6){
            struct DrawNode dw = {nodes[0].toDouble(), nodes[1].toDouble(), nodes[2].toDouble(), nodes[3].toDouble(), nodes[4].toDouble(), nodes[5].toDouble()};
            temp.append(dw);
        } else{
            if(l != ""){
                initAll();
                temp_path.clear();
                return false;
            }
        }
    }
    draw_node.append(temp);
    return true;
}

bool MainWindow::readTexts(QString data){
    QStringList lines = data.split("`");
    QVector<struct DrawText> temp;
    struct DrawNode temp_set;
    for(int i = 0; i < lines.size(); i ++){
        if(i % 2 == 1){
            lines[i].remove('\n');
            QStringList set = lines[i].split(" ");
            if(set.size() == 6){
                temp_set = {set[0].toDouble(), set[1].toDouble(), set[2].toDouble(), set[3].toDouble(), set[4].toDouble(), set[5].toDouble()};
            } else {
                if(lines[i] != ""){
                    initAll();
                    temp_path.clear();
                    return false;
                }
            }
        } else {
            DrawText dt = {temp_set, lines[i]};
            temp.append(dt);
        }
    }
    draw_text.append(temp);
    return true;
}

bool MainWindow::callCompiler(){
    QString codes = ce->toPlainText();
    if(codes != ""){
        if(codes.right(1) != ";"){
            codes += ";";
        }
        Compiler compiler(codes);
        connect(&compiler, &Compiler::clearSent, this, &MainWindow::clearReceive);
        connect(&compiler, &Compiler::processSent, this, &MainWindow::processReceive);
        connect(&compiler, &Compiler::resultSent, this, &MainWindow::resultReceive);
        connect(&compiler, &Compiler::errorSent, this, &MainWindow::errorReceive);
        return compiler.compile();
    } else{
        QMessageBox::critical(nullptr, "错误", "代码为空", QMessageBox::Yes);
        return false;
    }
    if(cb_auto_output_nodes->isChecked()){
        if(file_path.isValid() && file_path.url() != ""){
            try {
                Compiler::outputNodeData(file_path);
                return true;
            } catch (const std::exception& e) {
                QMessageBox::critical(nullptr, "失败", QString::fromStdString(e.what()), QMessageBox::Yes);
                return false;
            }
        } else{
            QMessageBox::critical(nullptr, "错误", "代码文件地址获取失败", QMessageBox::Yes);
            return false;
        }
    }
    return true;
}

void MainWindow::callDraw(){
    if(dw != NULL){
        delete dw;
        dw = NULL;
    }
    int x = draw_node.at(0).x;
    int y = draw_node.at(0).y;
    DrawWidget::countSize(x, y);
    dw = new DrawWidget();
    dw->resize(x, y);
    dw->show();
}

void MainWindow::changeLastLineColor(Qt::GlobalColor color) {
    if (!te_result) return;
    QTextDocument *document = te_result->document();
    if (!document) return;
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine);
    QTextCharFormat format;
    format.setForeground(QColor(color));
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.mergeCharFormat(format);
}

void MainWindow::outputPicture(QString format){
    try{
        if((draw_node.size() > 0 || draw_text.size() > 0)){
            if(!file_path.isValid() || file_path.url() == "" || QFileInfo(file_path.url()).suffix() != "d"){
                if(temp_path.size() > 0){
                    if(temp_path.size() == 1){
                        DrawWidget::outputPixmap(temp_path, format, DrawWidget::SINGLE);
                    } else{
                        DrawWidget::outputPixmap(temp_path, format, DrawWidget::MULTIPLE);
                    }
                }
            } else{
                QList<QUrl> temp;
                temp.append(file_path);
                DrawWidget::outputPixmap(temp, format, DrawWidget::SINGLE);
            }
        }

    } catch(const std::exception &e){
        QMessageBox::critical(nullptr, "失败", QString::fromStdString(e.what()), QMessageBox::Yes);
    }
}

void MainWindow::on_a_new_file_triggered()
{
    MainWindow *newWindow = new MainWindow();
    newWindow->show();
}


void MainWindow::on_a_open_file_triggered()
{
    readyToReadFile(TEXT);
}


void MainWindow::on_a_save_flie_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), TEXT)){
        setCRAvail(QFileInfo(file_path.url()).suffix());
    }
}


void MainWindow::on_a_save_as_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), AS)){
        setCRAvail(QFileInfo(file_path.url()).suffix());
    }
}

void MainWindow::on_a_font_triggered()
{
    bool ok;
    QFont myfont = QFontDialog::getFont(&ok, ce->font());
    if(ok){
        ce->setFont(myfont);
    }
}


void MainWindow::on_a_color_triggered()
{
    QColor color =QColorDialog::getColor();
    te_result->setTextColor(color);
}


void MainWindow::on_a_compile_triggered()
{
    if(!file_path.isValid() || file_path.url() == "" || readyToSaveFile(ce->toPlainText(), CODE)){
        te_result->show();
        callCompiler();
        setOutputAvail(true);
    }
}


void MainWindow::on_a_run_triggered()
{
    if(!file_path.isValid() || file_path.url() == "" || QFileInfo(file_path.url()).suffix() != "d"){
        if(readyToReadFile(NODE)){
            callDraw();
            setOutputAvail(true);
        }
    } else{
        if(draw_node.size() > 0 || draw_text.size() > 0 || readyToReadFile(NODE_THIS)){
            callDraw();
            setOutputAvail(true);
        }
    }

}


void MainWindow::on_a_compile_and_run_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), CODE)){
        te_result->show();
        if(callCompiler()){
            callDraw();
        }
        setOutputAvail(true);
    }
}


void MainWindow::on_a_output_JPG_triggered()
{
    outputPicture("JPG");
}


void MainWindow::on_a_output_PNG_triggered()
{
    outputPicture("PNG");
}


void MainWindow::on_a_output_BMP_triggered()
{
    outputPicture("BMP");
}


void MainWindow::on_a_output_nodes_triggered()
{
    try{
        if((draw_node.size() > 0 || draw_text.size() > 0) && status == COMPILED){
            if(!file_path.isValid() || file_path.url() == "" || QFileInfo(file_path.url()).suffix() != "d"){
                throw std::runtime_error("代码文件地址获取失败");
            } else{
                Compiler::outputNodeData(file_path);
            }
        }
        QMessageBox::information(nullptr, "成功", "文件导出成功", QMessageBox::Yes);
    } catch(const std::exception &e){
        QMessageBox::critical(nullptr, "失败", QString::fromStdString(e.what()), QMessageBox::Yes);
    }
}


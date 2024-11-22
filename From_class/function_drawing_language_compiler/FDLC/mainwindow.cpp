#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scanner.h"
#include "parsers.h"

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
            ui->l_text_file_status_display->setText("未保存");
        }
        jud_status_change = true;
    });

    setCREnabled(false);
    status = UNCOMPILED;

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
}

void MainWindow::errorReceive(QString error, Compiler::Status c_status, Scanner::Status s_status, Parsers::Status p_status){
    QString compiler_sataus;
    QString scanner_status;
    QString parsers_status;
    switch(c_status){
    case Compiler::PREPARING: compiler_sataus = "PREPARING"; break;
    case Compiler::START: compiler_sataus = "START"; break;
    case Compiler::SCANNING: compiler_sataus = "SCANNING"; break;
    case Compiler::PARSERING: compiler_sataus = "PARSERING"; break;
    case Compiler::SUCCEED: compiler_sataus = "SUCCEED"; break;
    case Compiler::FAILED: compiler_sataus = "FAILED"; break;
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
    case Parsers::P_ORIGIN: parsers_status = "ORIGIN"; break;
    case Parsers::P_ROT: parsers_status = "ROT"; break;
    case Parsers::P_SCALE: parsers_status = "SCALE"; break;
    case Parsers::P_FOR: parsers_status = "FOR"; break;
    case Parsers::P_SUCCEED: parsers_status = "SUCCEED"; break;
    }
    QString error_string = "ERROR: " + error;
    QString status_string = "STATUS: <COMPILER-" + compiler_sataus + "> <SCANNER-" + scanner_status + "> <PARSERS-" + parsers_status + ">";
    QList<QString> err = error_string.split("\n");
    for(auto e : err){
        if(e != ""){
            te_result->append(e);
            changeLastLineColor(Qt::red);
        }
    }
    te_result->append(status_string);
    changeLastLineColor(Qt::red);
    status = UNCOMPILED;
}

void MainWindow::setCREnabled(bool jud){
    ui->a_compile->setEnabled(jud);
    ui->a_compile_and_run->setEnabled(jud);
    ui->a_color->setEnabled(!jud);
}

void MainWindow::setCREnabled(QString suffix){
    if(suffix != "d"){
        setCREnabled(false);
    } else{
        setCREnabled(true);
    }
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
            ui->l_text_file_status_display->setText("已保存");
            jud_status_change = false;
            te_result->hide();
            setCREnabled(QFileInfo(file_path.url()).suffix());
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
        if(jud == NODE){
            default_file_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator();
            filter =  "Draw Node Files (*.dn);;Draw Text Files (*.dt)";
            file_paths = QFileDialog::getOpenFileNames(nullptr, "打开多个文件", default_file_path, filter);
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
            for(auto fp : file_paths){
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
        }
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
    ui->l_text_file_status_display->setText("已保存");
    ui->l_text_current_file_display->setText(file_path.fileName());
    jud_status_change = true;
    return true;
}

bool MainWindow::readNodes(QString data){
    QStringList lines = data.split("\n");
    QVector<struct DrawNode> temp;
    for(auto l : lines){
        QStringList nodes = l.split(" ");
        if(nodes.size() == 5){
            struct DrawNode dw = {nodes[0].toDouble(), nodes[1].toDouble(), nodes[2].toDouble(), nodes[3].toDouble(), nodes[4].toDouble()};
            temp.append(dw);
        } else{
            if(l != ""){
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
            if(set.size() == 5){
                temp_set = {set[0].toDouble(), set[1].toDouble(), set[2].toDouble(), set[3].toDouble(), set[4].toDouble()};
            } else {
                if(lines[i] != ""){
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

void MainWindow::callCompiler(){
    QString codes = ce->toPlainText() + ";";
    if(codes != "" && file_path.isValid() && file_path.url() != ""){
        QThread* thread = new QThread();
        Compiler* compiler = new Compiler(codes, file_path);
        compiler->moveToThread(thread);
        connect(thread, &QThread::started, compiler, &Compiler::compile);
        connect(compiler, &Compiler::clearSent, this, &MainWindow::clearReceive);
        connect(compiler, &Compiler::processSent, this, &MainWindow::processReceive);
        connect(compiler, &Compiler::resultSent, this, &MainWindow::resultReceive);
        connect(compiler, &Compiler::errorSent, this, &MainWindow::errorReceive);
        connect(compiler, &Compiler::resultSent, compiler, &Compiler::deleteLater);
        connect(compiler, &Compiler::errorSent, compiler, &Compiler::deleteLater);
        connect(compiler, &Compiler::resultSent, thread, &QThread::quit);
        connect(compiler, &Compiler::errorSent, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
    } else{
        if(codes == ""){
            QMessageBox::critical(nullptr, "错误", "代码为空", QMessageBox::Yes);
        } else{
            QMessageBox::critical(nullptr, "错误", "代码文件地址获取失败", QMessageBox::Yes);
        }
    }
}

void MainWindow::callDraw(){
        if(dw != NULL){
            delete dw;
            dw = NULL;
        }
        dw = new DrawWidget();
        dw->resize(1000, 700);
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
        setCREnabled(QFileInfo(file_path.url()).suffix());
    }
}


void MainWindow::on_a_save_as_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), AS)){
        setCREnabled(QFileInfo(file_path.url()).suffix());
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

    if(readyToSaveFile(ce->toPlainText(), CODE)){
        te_result->show();
        callCompiler();
    }
}


void MainWindow::on_a_run_triggered()
{
    if(!file_path.isValid() || file_path.url() == "" || QFileInfo(file_path.url()).suffix() != "d"){
        if(readyToReadFile(NODE)){
            callDraw();
        }
    } else{
        if(draw_node.size() > 0 || draw_text.size() > 0){
            if(readyToReadFile(NODE_THIS)){
                callDraw();
            }
        }
    }

}


void MainWindow::on_a_compile_and_run_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), CODE)){
        te_result->show();
        callCompiler();
        callDraw();
    }
}





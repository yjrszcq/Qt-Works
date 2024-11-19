#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scanner.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QSplitter>
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parsersOutputReceive(const QString &text, Qt::GlobalColor color){
    te_result->append(text);
    changeLastLineColor(color);
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

bool MainWindow::readyToSaveFile(QString data, SaveType jud){
    if(!file_path.isValid() || file_path.url() == ""){
        QString defaultFileName, filter;
        switch(jud){
        case TEXT:{
            defaultFileName = "新文本文件.txt";
            filter =  "All Files (*);;Text Files (*.txt)";
            break;
        }
        case CODE:{
            defaultFileName = "new codes.d";
            filter =  "Draw Code Files (*.d)";
            break;
        }
        case OUTPUT: break;
        }
        QString default_file_path = QDir::currentPath() + QDir::separator() + defaultFileName;
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
    jud_status_change = true;
    return true;
}

QVector<Tokens> MainWindow::callScanner(){
    QVector<Tokens> token_stream;
    Scanner scanner(ce->toPlainText());
    Tokens token;
    token = scanner.getToken();
    token_stream.append(token);
    while (token.type != NONTOKEN) {
        token = scanner.getToken();
        token_stream.append(token);
    }
    outputScannerResult(token_stream);
    return token_stream;
}

Parsers* MainWindow::callParsers(QVector<Tokens> token_stream){
    Parsers::cnt = 0;
    Parsers* p = new Parsers(token_stream, this);
    connect(p, SIGNAL(parsersOutputSent(const QString, Qt::GlobalColor)), this, SLOT(parsersOutputReceive(const QString, Qt::GlobalColor)));
    te_result->append("------------------------------");
    te_result->append("Parsers Result");
    changeLastLineColor(Qt::blue);
    te_result->append("------------------------------");
    p->parser();
    return p;
}

void MainWindow::sts(const QString &type, const QString &lexeme, double value, const QString &func_ptr) {
    QString output = QString("%1\t|\t%2\t|\t%3\t|\t%4\n")
                         .arg(type, 10)
                         .arg(lexeme, 12)
                         .arg(value, 14, 'f', 6)
                         .arg(func_ptr, 16);
    te_result->append(output);
}

void MainWindow::stsOut(Tokens token) {
    switch (token.type) {
    case ORIGIN: sts("ORIGIN", token.lexeme, token.value, "NULL"); break;
    case SCALE: sts("SCALE", token.lexeme, token.value, "NULL"); break;
    case ROT: sts("ROT", token.lexeme, token.value, "NULL"); break;
    case IS: sts("IS", token.lexeme, token.value, "NULL"); break;
    case TO: sts("TO", token.lexeme, token.value, "NULL"); break;
    case STEP: sts("STEP", token.lexeme, token.value, "NULL"); break;
    case DRAW: sts("DRAW", token.lexeme, token.value, "NULL"); break;
    case FOR: sts("FOR", token.lexeme, token.value, "NULL"); break;
    case FROM: sts("FROM", token.lexeme, token.value, "NULL"); break;
    case T: sts("T", token.lexeme, token.value, "NULL"); break;
    case SEMICO: sts("SEMICO", token.lexeme, token.value, "NULL"); break;
    case L_BRACKET: sts("L_BRACKET", token.lexeme, token.value, "NULL"); break;
    case R_BRACKET: sts("R_BRACKET", token.lexeme, token.value, "NULL"); break;
    case COMMA: sts("COMMA", token.lexeme, token.value, "NULL"); break;
    case PLUS: sts("PLUS", token.lexeme, token.value, "NULL"); break;
    case MINUS: sts("MINUS", token.lexeme, token.value, "NULL"); break;
    case MUL: sts("MUL", token.lexeme, token.value, "NULL"); break;
    case DIV: sts("DIV", token.lexeme, token.value, "NULL"); break;
    case POWER: sts("POWER", token.lexeme, token.value, "NULL"); break;
    case FUNC: {
        QString temp = token.lexeme;
        temp = temp.toUpper();
        sts("FUNC", token.lexeme, token.value, temp);
    } break;
    case CONST_ID: sts("CONST_ID", token.lexeme, token.value, "NULL"); break;
    case NONTOKEN: sts("NONTOKEN", token.lexeme, token.value, "NULL"); break;
    case ERRTOKEN: sts("ERRTOKEN", token.lexeme, token.value, "NULL"); break;
    }
}

void MainWindow::outputScannerResult(QVector<Tokens> token_stream){
    te_result->show();
    te_result->clear();
    te_result->append("------------------------------");
    te_result->append("Scanner Result");
    changeLastLineColor(Qt::blue);
    te_result->append("------------------------------");
    for(auto tokens : token_stream){
        stsOut(tokens);
    }
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

void MainWindow::on_new_file_triggered()
{
    MainWindow *newWindow = new MainWindow();
    newWindow->show();
}


void MainWindow::on_open_file_triggered()
{
    QString default_file_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator();
    QString filter =  "All Files (*);;Text Files (*.txt)";
    file_path = QFileDialog::getOpenFileName(nullptr, "打开", default_file_path, filter);
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
        te_result->hide();
    } else{
        file_path.clear();
    }
}


void MainWindow::on_save_flie_triggered()
{
    readyToSaveFile(ce->toPlainText(), TEXT);
}


void MainWindow::on_font_triggered()
{
    bool ok;
    QFont myfont = QFontDialog::getFont(&ok, ce->font());
    if(ok){
        ce->setFont(myfont);
    }
}


void MainWindow::on_compile_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), CODE)){
        QVector<Tokens> token_stream = callScanner();
        Parsers* p = callParsers(token_stream);
        delete p;
    }
}


void MainWindow::on_color_triggered()
{
    QColor color =QColorDialog::getColor();
    te_result->setTextColor(color);
}


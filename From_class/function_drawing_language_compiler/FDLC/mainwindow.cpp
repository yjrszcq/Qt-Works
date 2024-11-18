#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    return token_stream;
}

void MainWindow::ts(const QString &type, const QString &lexeme, double value, const QString &func_ptr) {
    QString output = QString("%1\t|\t%2\t|\t%3\t|\t%4\n")
                         .arg(type, 10)
                         .arg(lexeme, 12)
                         .arg(value, 14, 'f', 6)
                         .arg(func_ptr, 16);
    te_result->append(output);
}

void MainWindow::tsOut(Tokens token) {
    switch (token.type) {
    case ORIGIN: ts("ORIGIN", token.lexeme, token.value, "NULL"); break;
    case SCALE: ts("SCALE", token.lexeme, token.value, "NULL"); break;
    case ROT: ts("ROT", token.lexeme, token.value, "NULL"); break;
    case IS: ts("IS", token.lexeme, token.value, "NULL"); break;
    case TO: ts("TO", token.lexeme, token.value, "NULL"); break;
    case STEP: ts("STEP", token.lexeme, token.value, "NULL"); break;
    case DRAW: ts("DRAW", token.lexeme, token.value, "NULL"); break;
    case FOR: ts("FOR", token.lexeme, token.value, "NULL"); break;
    case FROM: ts("FROM", token.lexeme, token.value, "NULL"); break;
    case T: ts("T", token.lexeme, token.value, "NULL"); break;
    case SEMICO: ts("SEMICO", token.lexeme, token.value, "NULL"); break;
    case L_BRACKET: ts("L_BRACKET", token.lexeme, token.value, "NULL"); break;
    case R_BRACKET: ts("R_BRACKET", token.lexeme, token.value, "NULL"); break;
    case COMMA: ts("COMMA", token.lexeme, token.value, "NULL"); break;
    case PLUS: ts("PLUS", token.lexeme, token.value, "NULL"); break;
    case MINUS: ts("MINUS", token.lexeme, token.value, "NULL"); break;
    case MUL: ts("MUL", token.lexeme, token.value, "NULL"); break;
    case DIV: ts("DIV", token.lexeme, token.value, "NULL"); break;
    case POWER: ts("POWER", token.lexeme, token.value, "NULL"); break;
    case FUNC: {
        QString temp = token.lexeme;
        temp = temp.toUpper();
        ts("FUNC", token.lexeme, token.value, temp);
    } break;
    case CONST_ID: ts("CONST_ID", token.lexeme, token.value, "NULL"); break;
    case NONTOKEN: ts("NONTOKEN", token.lexeme, token.value, "NULL"); break;
    case ERRTOKEN: ts("ERRTOKEN", token.lexeme, token.value, "NULL"); break;
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


void MainWindow::on_compile_triggered()
{
    te_result->show();
    te_result->clear();
    te_result->append("Scanner Result");
    te_result->append("------------------------------");
    QVector<Tokens> token_stream = callScanner();
    for(auto tokens : token_stream){
        tsOut(tokens);
    }
}


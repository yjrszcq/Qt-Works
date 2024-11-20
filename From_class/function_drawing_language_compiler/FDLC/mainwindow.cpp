#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scanner.h"
#include "parsers.h"

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

    setCREnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parsersOutputReceive(const QString &text, Qt::GlobalColor color){
    te_result->append(text);
    changeLastLineColor(color);
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
                        if(!readNodes(readFile(QUrl(fp)))){
                            QMessageBox::warning(nullptr, "警告", "有文件读取失败", QMessageBox::Yes);
                        }
                    } else if(QFileInfo(fp).suffix() == "dt"){
                        if(!readTexts(readFile(QUrl(fp)))){
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

void MainWindow::callScanner(){
    initAll();
    Scanner scanner(ce->toPlainText());
    Tokens token;
    token = scanner.getToken();
    token_stream.push_back(token);
    while (token.type != NONTOKEN) {
        token = scanner.getToken();
        token_stream.push_back(token);
    }
    outputScannerResult();
}

void MainWindow::sts(const QString &type, const QString &lexeme, double value, const QString &func_ptr) {
    QString output = QString("%1\t|\t%2\t|\t%3\t|\t%4\n").arg(type, 10).arg(lexeme, 12).arg(value, 14, 'f', 6).arg(func_ptr, 16);
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
    case FUNC: sts("FUNC", token.lexeme, token.value, token.lexeme.toUpper()); break;
    case CONST_ID: sts("CONST_ID", token.lexeme, token.value, "NULL"); break;
    case NONTOKEN: sts("NONTOKEN", token.lexeme, token.value, "NULL"); break;
    case ERRTOKEN: sts("ERRTOKEN", token.lexeme, token.value, "NULL"); break;
    case COLOR: sts("COLOR", token.lexeme, token.value, "NULL"); break;
    case NOTES: sts("NOTES", token.lexeme, token.value, "NULL"); break;
    case COLON: sts("COLON", token.lexeme, token.value, "NULL"); break;
    case QUOTES: sts("QUOTES", token.lexeme, token.value, "NULL"); break;
    }
}

void MainWindow::outputScannerResult(){
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

void MainWindow::callParsers(){
    Parsers::cnt = 0;
    Parsers p;
    connect(&p, SIGNAL(parsersOutputSent(const QString, Qt::GlobalColor)), this, SLOT(parsersOutputReceive(const QString, Qt::GlobalColor)));
    te_result->append("------------------------------");
    te_result->append("Parsers Result");
    changeLastLineColor(Qt::blue);
    te_result->append("------------------------------");
    p.parser();
    outputParsersResult();
}

void MainWindow::nodeTotalXY(QTextStream &out_total, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y){
    double x, y;
    parameter = start;
    if (step > 0) {
        while (parameter <= end) {
            x = Parsers::getExpValue(for_x);
            y = Parsers::getExpValue(for_y);
            nodeXY(out_total, x, y, origin_x, origin_y, scale_x, scale_y, rot_ang, r, g, b);
            parameter += step;
        }
    } else if (step < 0) {
        while (parameter >= end) {
            x = Parsers::getExpValue(for_x);
            y = Parsers::getExpValue(for_y);
            nodeXY(out_total, x, y, origin_x, origin_y, scale_x, scale_y, rot_ang, r, g, b);
            parameter += step;
        }
    }
}

void MainWindow::nodeXY(QTextStream &out_total, double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b){
    double temp_x, temp_y;
    x *= scale_x;
    y *= scale_y;
    temp_x = x;
    temp_y = y;
    x = temp_x * cos(rot_ang) + temp_y * sin(rot_ang);
    y = temp_y * cos(rot_ang) - temp_x * sin(rot_ang);
    x += origin_x;
    y += origin_y;
    out_total << QString::number(x) << " " << QString::number(y) << " " << QString::number(r) << " " << QString::number(g) << " " << QString::number(b) << "\n";
    struct DrawNode dn = {x, y, r, g, b};
    draw_node.append(dn);
}

void MainWindow::outTextXY(QTextStream &out_text, double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b) {
    out_text << QString::number(notes_x) << " " << QString::number(notes_y) << " " << QString::number(notes_r) << " " << QString::number(notes_g) << " " << QString::number(notes_b) << "\n";
    out_text << "`" << notes_string << "`\n";
    struct DrawNode dn = {notes_x, notes_y, notes_r, notes_g, notes_b};
    struct DrawText dt = {dn, notes_string};
    draw_text.append(dt);
}

void MainWindow::outputParsersResult(){
    QFileInfo fileInfo(file_path.url());
    QString baseName = fileInfo.baseName();
    if(start_values.size() > 0){
        QUrl out_total_path(fileInfo.absolutePath() + "/" + baseName + ".dn");
        QFile file_total(out_total_path.url());
        if (!file_total.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(nullptr, "错误", "无法导出分析文件" + baseName + ".dn", QMessageBox::Yes);
            return;
        }
        QTextStream out_total(&file_total);
        for (int i = 0; i < start_values.size(); ++i) {
            nodeTotalXY(out_total, origin_x_values[i], origin_y_values[i], scale_x_values[i], scale_y_values[i], rot_ang_values[i], r_values[i], g_values[i], b_values[i], start_values[i], end_values[i], step_values[i], for_x_values[i], for_y_values[i]);
        }
        file_total.close();
    }
    if(notes_string_values.size() > 0){
        QUrl out_text_path(fileInfo.absolutePath() + "/" + baseName + ".dt");
        QFile file_text(out_text_path.url());
        if (!file_text.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(nullptr, "错误", "无法导出分析文件" + baseName + ".dt", QMessageBox::Yes);
            return;
        }
        QTextStream out_text(&file_text);
        for (int i = 0; i < notes_string_values.size(); ++i) {
            outTextXY(out_text, notes_x_values[i], notes_y_values[i], notes_string_values[i], notes_r_values[i], notes_g_values[i], notes_b_values[i]);
        }
        file_text.close();
    }
}

void MainWindow::callDraw(){
    if(dd != NULL){
        delete dd;
        dd = NULL;
    }
    dd = new DrawDialog();
    dd->resize(1000, 700);
    dd->exec();
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
        callScanner();
        callParsers();
    }
}


void MainWindow::on_a_run_triggered()
{
    if(!file_path.isValid() || file_path.url() == "" || QFileInfo(file_path.url()).suffix() != "d"){
        initAll();
        if(!readyToReadFile(NODE)){
            return;
        }
    } else{
        if(!(start_values.size() > 0 || notes_string_values.size() > 0)){
            if(!readyToReadFile(NODE_THIS)){
                return;
            }
        }
    }
    callDraw();
}


void MainWindow::on_a_compile_and_run_triggered()
{
    if(readyToSaveFile(ce->toPlainText(), CODE)){
        te_result->show();
        callScanner();
        callParsers();
        callDraw();
    }
}





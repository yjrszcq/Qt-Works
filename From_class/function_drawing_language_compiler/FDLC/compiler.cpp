#include "compiler.h"

#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>

Compiler::Compiler(QString codes, QUrl file_path, QObject *parent)
    : QObject{parent}
{
    this->codes = codes;
    this->file_path = file_path;
    status = PREPARING;
    scanner_status = Scanner::S_PREPARING;
    parsers_status = Parsers::P_PREPARING;
}

bool Compiler::run(QString codes, QUrl file_path){
    if(codes != "" && file_path.isValid() && file_path.url() != ""){
        Compiler compiler(codes, file_path);
        if(compiler.compile()){
            return true;
        }
    }
    return false;
}

bool Compiler::compile(){
    try{
        QMutexLocker locker(&mutex);
        status = START;
        callScanner();
        scanner_status = Scanner::S_SUCCEED;
        callParsers();
        parsers_status = Parsers::P_SUCCEED;
        locker.unlock();
        emit resultSent();
        status = SUCCEED;
    } catch(const std::exception &e) {
        emit errorSent(QString::fromStdString(e.what()), status, scanner_status, parsers_status);
        status = FAILED;
        initAll();
        return false;
    }
    return true;
}

void Compiler::scannerStatusReceive(Scanner::Status scanner_status){
    this->scanner_status = scanner_status;
}

void Compiler::parsersStatusReceive(Parsers::Status parsers_status){
    this->parsers_status = parsers_status;
}

void Compiler::parsersOutputReceive(const QString &text, Qt::GlobalColor color){
    emit processSent(text, color);
}

void Compiler::callScanner(){
    try{
        status = SCANNING;
        initAll();
        Scanner scanner(codes);
        connect(&scanner, SIGNAL(scannerStatusSent(Scanner::Status)), this, SLOT(scannerStatusReceive(Scanner::Status)));
        Tokens token;
        token = scanner.getToken();
        token_stream.push_back(token);
        while (token.type != NONTOKEN) {
            token = scanner.getToken();
            token_stream.push_back(token);
        }
        outputScannerResult();
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

}

void Compiler::sts(const QString &type, const QString &lexeme, double value, const QString &func_ptr) {
    QString output = QString("%1\t|\t%2\t|\t%3\t|\t%4\n").arg(type, 10).arg(lexeme, 12).arg(value, 14, 'f', 6).arg(func_ptr, 16);
    emit processSent(output, Qt::black);
}

void Compiler::stsOut(Tokens token) {
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

void Compiler::outputScannerResult(){
    emit refreshSent();
    emit processSent("------------------------------", Qt::black);
    emit processSent("Scanner Result", Qt::blue);
    emit processSent("------------------------------", Qt::black);
    for(auto tokens : token_stream){
        stsOut(tokens);
    }
}

void Compiler::callParsers(){
    try{
        status = PARSERING;
        Parsers::cnt = 0;
        Parsers p;
        connect(&p, SIGNAL(parsersStatusSent(Parsers::Status)), this, SLOT(parsersStatusReceive(Parsers::Status)));
        connect(&p, SIGNAL(parsersOutputSent(const QString, Qt::GlobalColor)), this, SLOT(parsersOutputReceive(const QString, Qt::GlobalColor)));
        emit processSent("------------------------------", Qt::black);
        emit processSent("Parsers Result", Qt::blue);
        emit processSent("------------------------------", Qt::black);
        p.parser();
        outputParsersResult();
    } catch(const std::exception &e){
        throw std::runtime_error(std::string(e.what()));
    }
}

void Compiler::nodeTotalXY(QTextStream &out_total, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y){
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

void Compiler::nodeXY(QTextStream &out_total, double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b){
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

void Compiler::outTextXY(QTextStream &out_text, double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b) {
    out_text << QString::number(notes_x) << " " << QString::number(notes_y) << " " << QString::number(notes_r) << " " << QString::number(notes_g) << " " << QString::number(notes_b) << "\n";
    out_text << "`" << notes_string << "`\n";
    struct DrawNode dn = {notes_x, notes_y, notes_r, notes_g, notes_b};
    struct DrawText dt = {dn, notes_string};
    draw_text.append(dt);
}

void Compiler::outputParsersResult(){
    QFileInfo fileInfo(file_path.url());
    QString baseName = fileInfo.baseName();
    if(start_values.size() > 0){
        try{
            QUrl out_total_path(fileInfo.absolutePath() + "/" + baseName + ".dn");
            QFile file_total(out_total_path.url());
            if (!file_total.open(QIODevice::WriteOnly | QIODevice::Text)) {
                throw std::runtime_error(("无法导出分析文件" + baseName + ".dn").toStdString());
            }
            QTextStream out_total(&file_total);
            for (int i = 0; i < start_values.size(); ++i) {
                nodeTotalXY(out_total, origin_x_values[i], origin_y_values[i], scale_x_values[i], scale_y_values[i], rot_ang_values[i], r_values[i], g_values[i], b_values[i], start_values[i], end_values[i], step_values[i], for_x_values[i], for_y_values[i]);
            }
            file_total.close();
        } catch(const std::exception &e){
            throw std::runtime_error(std::string(e.what()));
        }

    }
    if(notes_string_values.size() > 0){
        try{
            QUrl out_text_path(fileInfo.absolutePath() + "/" + baseName + ".dt");
            QFile file_text(out_text_path.url());
            if (!file_text.open(QIODevice::WriteOnly | QIODevice::Text)) {
                throw std::runtime_error(("无法导出分析文件" + baseName + ".dt").toStdString());
            }
            QTextStream out_text(&file_text);
            for (int i = 0; i < notes_string_values.size(); ++i) {
                outTextXY(out_text, notes_x_values[i], notes_y_values[i], notes_string_values[i], notes_r_values[i], notes_g_values[i], notes_b_values[i]);
            }
            file_text.close();
        } catch(const std::exception &e){
            throw std::runtime_error(std::string(e.what()));
        }
    }
}

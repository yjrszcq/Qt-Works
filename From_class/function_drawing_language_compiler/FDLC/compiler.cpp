#include "compiler.h"
#include "globaldefines.h"

#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>

Compiler::Compiler(QString codes, QObject *parent)
    : QObject{parent}
{
    this->codes = codes;
    error_tokens_str = "";
    status = C_PREPARING;
    scanner_status = Scanner::S_PREPARING;
    parsers_status = Parsers::P_PREPARING;
}

bool Compiler::run(QString codes){
        Compiler compiler(codes);
        if(compiler.compile()){
            return true;
        }
    return false;
}

bool Compiler::compile(){
    try{
        QMutexLocker locker(&mutex);
        status = C_START;
        emit clearSent();
        callScanner();
        callParsers();
        emit resultSent();
        locker.unlock();
        status = C_SUCCEED;
    } catch(const std::exception &e) {
        emit errorSent(QString::fromStdString(e.what()), status, scanner_status, parsers_status);
        status = C_FAILED;
        initAll();
        return false;
    }
    return true;
}

void Compiler::scannerStatusReceive(Scanner::Status scanner_status){
    this->scanner_status = scanner_status;
}

void Compiler::scannerOutputReceive(const QString &text, Qt::GlobalColor color, const QString error_token_lexeme){
    if(color == Qt::red){
        error_tokens_str.append("\nUnkonwn Token: " + error_token_lexeme);
    }
    emit processSent(text, color);
}

void Compiler::parsersStatusReceive(Parsers::Status parsers_status){
    this->parsers_status = parsers_status;
}

void Compiler::parsersOutputReceive(const QString &text, Qt::GlobalColor color){
    emit processSent(text, color);
}

void Compiler::callScanner(){
    try{
        status = C_SCANNING;
        initAll();
        Scanner scanner(codes);
        emit processSent("------------------------------", Qt::black);
        emit processSent("Scanner Result", Qt::blue);
        emit processSent("------------------------------", Qt::black);
        connect(&scanner, SIGNAL(scannerStatusSent(Scanner::Status)), this, SLOT(scannerStatusReceive(Scanner::Status)));
        connect(&scanner, SIGNAL(scannerOutputSent(const QString, Qt::GlobalColor, const QString)), this, SLOT(scannerOutputReceive(const QString, Qt::GlobalColor, const QString)));
        scanner.scan();
        outputScannerError();
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
}

void Compiler::outputScannerError(){
    try{
        if(error_tokens_str != ""){
            qDebug() << 1;
            throw std::runtime_error(error_tokens_str.toStdString());
        }
    } catch(const std::exception &e){
        throw std::runtime_error(std::string(e.what()));
    }
}

void Compiler::callParsers(){
    try{
        status = C_PARSERING;
        Parsers::cnt = 0;
        Parsers p;
        connect(&p, SIGNAL(parsersStatusSent(Parsers::Status)), this, SLOT(parsersStatusReceive(Parsers::Status)));
        connect(&p, SIGNAL(parsersOutputSent(const QString, Qt::GlobalColor)), this, SLOT(parsersOutputReceive(const QString, Qt::GlobalColor)));
        emit processSent("------------------------------", Qt::black);
        emit processSent("Parsers Result", Qt::blue);
        emit processSent("------------------------------", Qt::black);
        p.parser();
    } catch(const std::exception &e){
        throw std::runtime_error(std::string(e.what()));
    }
}

void Compiler::outputNodeData(QUrl file_path){
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
            for (int i = 0; i < draw_node.size(); ++i) {
                out_total << QString::number(draw_node.at(i).x) << " " << QString::number(draw_node.at(i).y) << " " << QString::number(draw_node.at(i).r) << " " << QString::number(draw_node.at(i).g) << " " << QString::number(draw_node.at(i).b) << " " << QString::number(draw_node.at(i).pix) << "\n";

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
            for (int i = 0; i < draw_text.size(); ++i) {
                out_text << QString::number(draw_text.at(i).set.x) << " " << QString::number(draw_text.at(i).set.y) << " " << QString::number(draw_text.at(i).set.r) << " " << QString::number(draw_text.at(i).set.g) << " " << QString::number(draw_text.at(i).set.b) << " " << QString::number(draw_text.at(i).set.pix) << "\n";
                out_text << "`" << draw_text.at(i).str << "`\n";
            }
            file_text.close();
        } catch(const std::exception &e){
            throw std::runtime_error(std::string(e.what()));
        }
    }
}

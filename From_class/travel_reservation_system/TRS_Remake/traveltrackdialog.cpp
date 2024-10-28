#include "traveltrackdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

TravelTrackDialog::TravelTrackDialog(QList<QHash<QString,QString>> resv_data, Server *server) {
    this->setWindowTitle("我的旅行路线");
    this->server = server;

    pb_ok = new QPushButton("确认");
    pb_check = new QPushButton("检查完整性");

    pb_check->setEnabled(false);

    te_analyze = new QTextEdit();

    te_analyze->setReadOnly(true);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(pb_ok);
    hbox->addWidget(pb_check);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(te_analyze);
    vbox->addLayout(hbox);

    this->setLayout(vbox);

    connect(pb_ok, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(pb_check, SIGNAL(clicked(bool)), this, SLOT(outputCompletenessAnalyze()));

    outputTrack(getContentData(resv_data, server));

}

int TravelTrackDialog::getContentData(QList<QHash<QString,QString>> resv_data, Server *server){
    QList<QHash<QString,QString>> flight = server->getData(0);
    QList<QHash<QString,QString>> hotel = server->getData(1);
    QList<QHash<QString,QString>> bus = server->getData(2);
    if(flight.size() != 0){
        bool jud = false;
        QList<QHash<QString,QString>> avail_resv;
        for(auto rd : resv_data){
            if(rd["resvAvail"] == QString::number(Reservation::AVAILABLE)){
                avail_resv.append(rd);
                if(rd["resvType"] == QString::number(Reservation::FLIGHT)){
                    jud = true;
                }
            }
        }
        if(jud == true){
            bool ok;
            for(auto ar : avail_resv){
                switch(ar["resvType"].toInt(&ok, 10)){
                case Reservation::FLIGHT:{
                    for(auto f : flight){
                        if(ar["resvContent"] == f["flightNum"]){
                            my_flight.append(f);
                        }
                    }
                    break;
                }
                case Reservation::HOTEL:{
                    for(auto h : hotel){
                        if(ar["resvContent"] == h["location"]){
                            my_hotel.append(h);
                        }
                    }
                    break;
                }
                case Reservation::BUS:{
                    for(auto b : bus){
                        if(ar["resvContent"] == b["location"]){
                            my_bus.append(b);
                        }
                    }
                }
                }
            }
            return 0;
        } else{
            return 1;
        }
    } else{
        return 2;
    }
}

QList<QString> TravelTrackDialog::travelTrackAnalyze() {
    QList<QList<QString>> line;
    for(auto mf : my_flight){
        QList<QString> temp;
        temp.append(mf["FromCity"]);
        temp.append(mf["ArivCity"]);
        line.append(temp);
    }
    bool jud[line.size()], jud_ring[line.size()];
    for(int i = 0; i < line.size(); i ++){
        jud[i] = true;
        jud_ring[i] = false;
    }
    for(int i = 0; i < line.size(); i ++){
        bool jud_again = false;
        for(int j = 0; j < line.size(); j ++){
            if(line[i].last() == line[j][0] && !jud_ring[j]){ // 两个list头尾相同, 将后面的list整个接到前面的list
                if(i == j){ // i = j则成环
                    jud_ring[i] = true;
                    break;
                }
                for(int k = 1; k < line[j].size(); k ++){
                    line[i].append(line[j][k]);
                }
                jud[j] = false; // 后面的list不输出
                jud_again = true; // 重扫一遍
            }
        }
        if(jud_again){
            i --;
        }
    }
    QList<QString> result;
    int j = 1;
    for(int i = 0; i < line.size(); i ++){
        if(jud[i]){
            QString temp = "路线 " + QString::number(j ++);
            for(auto &l : line[i]){
                temp += " -> " + l;
            }
            result.append(temp);
        }
    }
    return result;
}


void TravelTrackDialog::outputTrack(int flag){
    switch(flag){
    case 0: {
        track_result = travelTrackAnalyze();
        te_analyze->append("旅行路线");
        te_analyze->append("-----");
        for(auto tr : track_result){
            te_analyze->append(tr);
        }
        pb_check->setEnabled(true);
        break;
    }
    case 1: {
        te_analyze->append("旅行路线获取失败");
        te_analyze->append("-----");
        te_analyze->append("您没有预订航班，无法分析旅行路线");
        break;
    }
    case 2: {
        te_analyze->append("错误");
        te_analyze->append("-----");
        te_analyze->append("航班数据为空");
        break;
    }
    }
}

QList<QString> TravelTrackDialog::resvCompletenessAnalyze(){
    QList<QString> result;
    for(auto tr : track_result){
        QList<QString> parts = tr.split(" -> ");
        QString tempResult = parts[0] + ": ";
        bool jud_tr = true;
        QList<QString> information;
        for(int i = 2; i < parts.size(); i ++){
            bool jud_h = false, jud_b = false;
            for(auto mh : my_hotel){
                if(parts[i] == mh["location"]){
                    jud_h = true;
                    break;
                }
            }
            for(auto mb : my_bus){
                if(parts[i] == mb["location"]){
                    jud_b = true;
                    break;
                }
            }
            if(!jud_h || !jud_b){
                jud_tr = false;
                QString tempInfo = "您在 '" + parts[i] + "' 没有预订 ";
                if(!jud_h){
                    tempInfo += "'宾馆' ";
                }
                if(!jud_b){
                    tempInfo += "'客车' ";
                }
                tempInfo += "\n";
                information.append(tempInfo);
            }
        }
        if(jud_tr){
            tempResult += "此路线预订完整\n";
        } else{
            tempResult += "此路线预订不完整\n";
            for(auto info : information){
                tempResult += info;
            }
        }
        result.append(tempResult);
    }
    return result;
}

void TravelTrackDialog::outputCompletenessAnalyze(){
    QList<QString> result = resvCompletenessAnalyze();
    te_analyze->append("-----");
    te_analyze->append("预订路线完整性分析");
    te_analyze->append("-----");
    for(auto r : result){
        te_analyze->append(r);
    }
    pb_check->setEnabled(false);
}

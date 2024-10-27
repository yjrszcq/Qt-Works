#ifndef TRAVELTRACKDIALOG_H
#define TRAVELTRACKDIALOG_H

#include "server.h"

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>

class TravelTrackDialog : public QDialog
{
    Q_OBJECT
public:
    TravelTrackDialog(QList<QHash<QString,QString>> resv_data, Server *server);
    static void show(QList<QHash<QString,QString>> resv_data, Server *server){
        TravelTrackDialog ttd(resv_data, server);
        ttd.exec();
    }

private:
    int getContentData(QList<QHash<QString,QString>> resv_data, Server *server);
    QList<QString> travelTrackAnalyze();
    QList<QString> resvCompletenessAnalyze();
    void outputTrack(int flag);

public slots:
    void outputCompletenessAnalyze();

private:
    Server *server;
    QList<QHash<QString,QString>> my_flight, my_hotel, my_bus;
    QPushButton *pb_ok, *pb_check;
    QTextEdit *te_analyze;
    QList<QString> track_result;
};

#endif // TRAVELTRACKDIALOG_H
